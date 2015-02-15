#include "common.h"
#include "packet.h"
#include "frame.h"
#include "resample.h"
#include "audiofifo.h"
#include "codeccontext.h"
#include "formatcontext.h"

void Frame::allocate() throw(const Error&) {
    f = av_frame_alloc();
    if(!f)
        throw Error("Couldn't allocate memory for frame.");
}

Frame::Frame():f(NULL),usable(false) {}

Frame::~Frame() {
    if(f)
        av_frame_free(&f);
}

void Frame::readFrame(IFormatContext& fc,const CodecContext& cc, int stream_id) throw(const Error&,const Empty&) {

    if(!f)
        throw Empty("Cannot read frame.","Frame");

    Packet pkt;
    int ret = 0;
    int ret_tot = 0;
    int complete = 0;

    // Added AVERROR_EOF to flush the decoder
    while( ((ret = av_read_frame(fc, pkt)) >= 0) || ret == AVERROR_EOF ) {

        if(pkt.streamIndex() != stream_id )
            continue;

        try {
            ret_tot += decoder(cc, pkt, complete);
        } catch(const Flush& f) {
            std::cout << '*';
        }
        if(complete || ret == AVERROR_EOF)
            break;
    }

    // I have another cleaner way to do this... I think
    if(ret < 0 ) {
        if (ret == AVERROR_EOF) {
            if(!complete) {
                streamEnd(fc);
                throw Error("End of stream");
            }
        } else
            throw Error("Couldn't read frame.");
    }

    // std::cout << ret_tot;
}

int VideoFrame::decoder(const CodecContext& cc,Packet& pkt, int& complete) throw(const Error&,const Flush&,const Empty&) {

    if(!f)
        throw Empty("Can't get the decoder","Frame");

    // Flush until no samples are returned
    if(pkt <= 0) {
        while ( avcodec_decode_video2(cc,f,&complete, pkt) > 0);
        throw Flush("Video Decoder");
    }
    // To read packets which have mutliple frames on the packet
    int ret =  avcodec_decode_video2(cc,f,&complete, pkt);
    if(ret < 0)
        throw Error("Couldn't decode frame.");
    return ret;
}

void VideoFrame::encoder(const CodecContext& cc, Packet& pkt)  throw(const Error&,const Flush&) {
    int complete = 0;
    if(usable) {
        int ret =  avcodec_encode_video2(cc, pkt,f, &complete);
        if(ret < 0 || complete==0 )
            throw Error("Couldn't encoder frame.");
    } else {
        //\std::cout << "Flushed" <<std::endl;
        avcodec_encode_video2(cc, pkt,NULL, &complete);
        throw Flush("Video Encoder");
        //complete = 1;
    }
}

void VideoFrame::streamEnd(IFormatContext& fc) {
    fc.setVideoEOS();
}

VideoFrame::~VideoFrame() {
    if(f && modified)
        avpicture_free(reinterpret_cast<AVPicture *>(f));
};

VideoFrame::VideoFrame(IFormatContext& fc,const VideoCodecContext& cc,const VideoCodecContext& oc,int w,int h) :Frame(),output_width(0),output_height(0),pan_x(0),pan_y(0),strip_x(0),strip_y(0),brightness_v(0.0),saturation_v(1.0),scale_ratio(0.0),effect_v(NONE),modified(false)  {
    try {

        if(fc.videoEOS())
            throw Error("Can't read frame. VideoStream is depleted.");

        Frame::allocate() ;

        readFrame(fc,cc,fc.videoStreamId() );

        usable = true;

        checkDeprecated( &f->format );

    // wxh is the real size of the image we require
    // Real widthxheight is the output widthxheight
    // Output widhtxheight is the scaled/optimized widthxheight;

    real_width = oc.width();
	real_height = oc.height();

    w = Maths::clip(Maths::limit(w,cc.width(),0),cc.width());
    h = Maths::clip(Maths::limit(h,cc.height(),0),cc.height());

     if( 1.0*real_width/real_height > 1.0*w/h){
		output_width = 1.0*real_height*w/h;
		output_height = real_height;
	}else{
		output_height = 1.0*real_width*h/w;
		output_width = real_width;
	}

    // Get the scale ratio
	scale_ratio = Maths::larger(1.0*output_width/w ,1.0*output_height/h);

    // Scale the image such that we get the scaled image with our required parts
     scale(AV_PIX_FMT_YUV420P, Maths::evenize(scale_ratio*cc.width()) , Maths::evenize(scale_ratio*cc.height()) );

    } catch(const Error& err) {
        usable = false;
        std::cout << 'x';
    }
}

VideoFrame::VideoFrame(const VideoFrame& vf):Frame(),modified(false){

    try{
        if(!vf)
            throw Empty("Couldn't copy video frame","Video Frame");


        Frame::allocate();

        f->format = vf.pixelFormat();
        f->width = vf.width();
        f->height = vf.height();

        int ret = avpicture_alloc(reinterpret_cast<AVPicture *>(f), vf.pixelFormat(), vf.width(), vf.height());
        if(ret < 0)
            throw Error("Couldn't allocate the picture.");
        modified = true;
        av_picture_copy(reinterpret_cast<AVPicture *>(f), (const AVPicture *) (AVFrame* )vf ,vf.pixelFormat(),vf.width(), vf.height());
        usable = true;

        output_width =vf.output_width,output_height =vf.output_height ,pan_x=vf.pan_x ,pan_y=vf.pan_y ,strip_x=vf.strip_x ,strip_y=vf.strip_y ,brightness_v=vf.brightness_v ,saturation_v=vf.saturation_v ,effect_v=vf.effect_v ;


        real_width = vf.real_width;
        real_height = vf.real_height;

        output_width = vf.output_width;
        output_height = vf.output_height;

        scale_ratio = vf.scale_ratio;

    } catch (const Error& err){
        usable = false;
        std::cout << 'x';
    }
}

void VideoFrame::effect(effects e){
    effect_v = e;
}

void VideoFrame::apply()  throw(const Error&,const Empty&) {

    if(!usable)
        throw Empty("Can't apply changes.","Frame");

    if( output_width == 0 && output_height==0 && pan_x==0 && pan_y==0 && strip_x==0 && strip_y==0 && brightness_v==0.0 && saturation_v==1.0 && effect_v==NONE){
        throw Error("Nothing to do.");
    }

    // Check if it isn't allocated and do the right thing
    if(!output_width && !output_height) {
        output_width = Maths::evenize(real_width);
        output_height = Maths::evenize(real_height);
    }

    // Find the size of the cropped image
    pan_x += Maths::evenize(strip_x/2);
    pan_y += Maths::evenize(strip_y/2);

    int image_width = Maths::evenize(Maths::smaller(output_width-strip_x,f->width-pan_x));
    int image_height = Maths::evenize(Maths::smaller(output_height-strip_y,f->height-pan_y));

    int gap_x = Maths::clip(Maths::evenize(real_width - image_width)/2 , 0);
    int gap_y = Maths::clip(Maths::evenize(real_height - image_height)/2 , 0);

//    std::cout << real_height << std::endl;
//    std::cout << image_height <<std::endl;
//    std::cout << gap_y <<std::endl;
//    exit(-1);


    AVFrame* frame = NULL;
    frame = av_frame_alloc();
    frame->format = f->format;
    frame->width = real_width;
    frame->height = real_height;
    int ret = avpicture_alloc( reinterpret_cast<AVPicture*>(frame), static_cast<AVPixelFormat>(frame->format), frame->width,frame->height);
    if(ret < 0)
        throw Error("Couldn't allocate the picture buffer.");

    //         Prepare a cropped image
    for(int y=0; y<real_height; y++) {
        for(int x=0; x<real_width; x++) {

            if(x >= gap_x && y >= gap_y && x < image_width+gap_x && y < image_height+gap_y) {
                frame->data[0][y * frame->linesize[0] + x] =f->data[0][(y-gap_y+pan_y) * f->linesize[0] + (x-gap_x+pan_x)];
                if(brightness_v) {
                    frame->data[0][y * frame->linesize[0] + x] = Maths::limit( frame->data[0][y * frame->linesize[0] + x] + brightness_v*255 , 255,0 );
                }
                if(effect_v == BW){
                    int threshold = 100;
                    if(frame->data[0][y * frame->linesize[0] + x] < threshold)
                        frame->data[0][y * frame->linesize[0] + x] = 0;
                    else
                        frame->data[0][y * frame->linesize[0] + x] = 255;
                }
            } else {
                frame->data[0][y * frame->linesize[0] + x] = 0;
            }

            if(y<real_height/2 && x<real_width/2) {

                if(x >= gap_x/2 && y >= gap_y/2 && x < (image_width+gap_x)/2 && y < (image_height+gap_y)/2) {

                    if(effect_v==GRAYSCALE || effect_v == BW){
                        frame->data[1][y * frame->linesize[1] + x] = 128;//128;
                        frame->data[2][y * frame->linesize[2] + x] = 128;//128;
                    }else{
                        frame->data[1][y * frame->linesize[1] + x] = f->data[1][(y+(-gap_y+pan_y)/2) * f->linesize[1] + (x+(-gap_x+pan_x)/2)];
                        frame->data[2][y * frame->linesize[2] + x] = f->data[2][(y+(-gap_y+pan_y)/2) * f->linesize[2] + (x+(-gap_x+pan_x)/2)];
                        if(saturation_v != 1.0 || brightness_v != 0.0 ) {
                            frame->data[1][y * frame->linesize[1] + x] =  (frame->data[1][y * frame->linesize[1] + x]-128)*Maths::limit(-Maths::absolute(brightness_v)+saturation_v,1.0,0.0) + 128 ;
                            frame->data[2][y * frame->linesize[2] + x] =  (frame->data[2][y * frame->linesize[2] + x]-128)*Maths::limit(-Maths::absolute(brightness_v)+saturation_v,1.0,0.0) + 128;
                        }
                    }



                } else {
                    frame->data[1][y * frame->linesize[1] + x] = 128;//128;
                    frame->data[2][y * frame->linesize[2] + x] = 128;//128;

                }

            }

        }
    }

    if(modified){
        avpicture_free(reinterpret_cast<AVPicture *>(f));
        av_frame_free(&f);
    }

    f = frame;
    modified = true;
    output_width = 0,output_height=0,pan_x=0,pan_y=0,strip_x=0,strip_y=0,brightness_v=0.0,saturation_v=1.0,effect_v = NONE;
}

void VideoFrame::scale( AVPixelFormat pix_fmt, int width, int height) throw(const Error&,const Empty&) {

    if(!usable)
        throw Empty("Can't scale the image","Frame");

    width = Maths::evenize(width > 0 ? width: f->width);
    height = Maths::evenize(height > 0 ? height : f->height);
    pix_fmt = (pix_fmt == AV_PIX_FMT_NONE) ? static_cast<AVPixelFormat>(f->format) : pix_fmt;

    AVFrame* frame = NULL;
    frame = av_frame_alloc();
    frame->format = pix_fmt;
    frame->width = width;
    frame->height = height;

    int ret = avpicture_alloc(reinterpret_cast<AVPicture *>(frame), pix_fmt, width,height);
    if(ret < 0)
        throw Error("Couldn't allocate the picture buffer.");
    modified = true;

    struct SwsContext *sws_ctx;
    sws_ctx = sws_getContext(f->width,f->height, static_cast< AVPixelFormat>(f->format),width,height, static_cast< AVPixelFormat>(pix_fmt),SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx)
        throw Error("Couldn't create scale context for the conversion.");



    sws_scale(sws_ctx, const_cast<const uint8_t* const*>(f->data) , f->linesize , 0, f->height, frame->data, frame->linesize);
    sws_freeContext(sws_ctx);

    av_frame_free(&f);
    f = frame;

    // std::cout <<"Message: Image scaled." << std::endl;
}

int AudioFrame::decoder(const CodecContext& cc,Packet& pkt,int& complete) throw(const Error&,const Empty&, const Flush&) {

    if(!f)
        throw Empty("Can't get the decoder","Frame");

    // Flush until no samples are returned
    if(pkt <= 0) {
        while ( avcodec_decode_audio4(cc,f,&complete, pkt) > 0);
        throw Flush("Audio Decoder");
    }
    // To read packets which have mutliple frames on the packet
    int ret =  avcodec_decode_audio4(cc,f,&complete, pkt);
    if(ret < 0)
        throw Error("Couldn't decode frame.");
    return ret;
}

void AudioFrame::set(const AudioCodecContext& occ,int nb_samples) throw(const Error&,const Empty&) {

    if(!f)
        throw Empty("Can't get the decoder","Frame");

    // Helps to flush the encoder
    if(nb_samples <= 0)
        throw Error("Couldn't set the AudioFrame. Number of samples is invalid");


    /**
     * Set the frame's parameters, especially its size and format.
     * av_frame_get_buffer needs this to allocate memory for the
     * audio samples of the frame.
     * Default channel layouts based on the number of channels
     * are assumed for simplicity.
     */
    f->nb_samples     = nb_samples;
    f->channel_layout = occ.channelLayout();
    f->format         = occ.sampleFormat();
    f->sample_rate    = occ.sampleRate();

    /**
     * Allocate the samples of the created frame. This call will make
     * sure that the audio frame can hold as many samples as specified.
     */
    int ret = av_frame_get_buffer(f, 0);
    if(ret < 0)
        throw Error("Couldn't allocate output frame samples.");

}

void AudioFrame::resample(const ResampleContext& resample_context,const AudioCodecContext& output_codec_context) throw(const Error&,const Empty&) {

    if(!usable)
        throw Empty("Can't resample the image.","Frame");

    AVFrame* frame = NULL;
    frame = av_frame_alloc();

    av_samples_alloc(frame->extended_data, NULL,output_codec_context.channels(),f->nb_samples,output_codec_context.sampleFormat(), 0);

    // Convert the samples using the resampler.
    int ret = swr_convert(resample_context,frame->extended_data, f->nb_samples,const_cast<const uint8_t **>(f->extended_data) , f->nb_samples);
    if (ret < 0)
        throw Error("Couldn't convert input samples.");

    av_samples_copy(f->extended_data, frame->extended_data, 0,0, f->nb_samples, output_codec_context.channels(),output_codec_context.sampleFormat());

    av_freep(&(frame->extended_data[0]));
    av_frame_free(&frame);

}

void AudioFrame::encoder(const CodecContext& cc, Packet& pkt) throw(const Error&,const Flush&) {

    int complete = 0;
    if(usable) {
        int ret =  avcodec_encode_audio2(cc, pkt,f, &complete);
        if(ret < 0)
            throw Error("Couldn't encode frame.");
    } else {
        //std::cout << "Flushed" <<std::endl;
        avcodec_encode_audio2(cc, pkt,NULL, &complete);
        throw Flush("Audio Encoder");
        //complete = 1;
    }

}

void AudioFrame::streamEnd(IFormatContext& fc) {
    fc.setAudioEOS();
}

AudioFrame::AudioFrame():Frame(),silence(false) {}


AudioFrame::AudioFrame(IFormatContext& fc,const AudioCodecContext& cc, const AudioCodecContext& occ, const ResampleContext& resample_ctx, AudioFIFO& fifo):Frame(),silence(false)  {

    try {
        Frame::allocate();

        if(fc.audioEOS()) {

           /**
            * Use the maximum number of possible samples per frame.
            * If there is less than the maximum possible frame size in the FIFO
            * buffer use this number. Otherwise, use the maximum possible frame size
            */


            int nb_samples =  occ.capabilities() & CODEC_CAP_VARIABLE_FRAME_SIZE ? 10000 : occ.frameSize();
            set(occ,nb_samples);
            av_samples_set_silence(f->extended_data, 0, f->nb_samples,occ.channels(),occ.sampleFormat());
            usable = true;



        } else {


            while(fifo.size() < occ.frameSize()) {

                usable = false;

//                try{
                readFrame(fc,cc,fc.audioStreamId());

                usable = true;

                /** Convert the input frame accoriding to resampling context */
                resample(resample_ctx,occ);

//                }catch(const EOS& e){
//                    // Put this for smooth change
//                    int nb_samples =  occ.capabilities() & CODEC_CAP_VARIABLE_FRAME_SIZE ? 10000 : occ.frameSize();
//                    set(occ,nb_samples);
//                    av_samples_set_silence(f->extended_data, 0, f->nb_samples,occ.channels(),occ.sampleFormat());
//                    usable = true;
//                }

                /** Add the converted input samples to the FIFO buffer for later processing. */
                fifo.put(*this);

            }

            /** Temporary storage of the output samples of the frame written to the file. */
            av_frame_free(&f);
            f = NULL;
            usable = false;

            /** Create a new frame to store the audio samples. */
            Frame::allocate();

            /**
            * Use the maximum number of possible samples per frame.
            * If there is less than the maximum possible frame size in the FIFO
            * buffer use this number. Otherwise, use the maximum possible frame size
            */
            int nb_samples = Maths::smaller(fifo.size(), occ.frameSize());
            set(occ,nb_samples);
            usable = true;


            /**
             * Read as many samples from the FIFO buffer as required to fill the frame.
             * The samples are stored in the frame temporarily.
             */


            fifo.get(*this);
        }

    } catch(const Error& err) {
        usable = false;
        std::cout << "x";
    }

}

AudioFrame::~AudioFrame() {
    if(f && silence)
        av_freep(&f->extended_data[0]);
}
