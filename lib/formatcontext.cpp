#include "common.h"
#include "packet.h"
#include "frame.h"
#include "stream.h"
#include "codeccontext.h"
#include "formatcontext.h"


FormatContext::FormatContext():fc(NULL) {}

FormatContext::~FormatContext() {
    if(fc)
        avformat_free_context(fc);
}


AVStream* FormatContext::streams(unsigned int i)const throw(const Error&) {
    if(fc->nb_streams<=0)
        throw Error("There are no streams.");
    if(i > fc->nb_streams)
        throw Error("No streams found with the id");

    return fc->streams[i];
}


IFormatContext::IFormatContext(const Chary& file_name) :FormatContext(),audio_complete(false),video_complete(false) {

    if( fileExists(file_name) != 1)
        throw Error("File '" +file_name +"' doesn't exist.");

    /// Open an input stream and read the header.
    int ret = avformat_open_input(&fc, file_name , NULL, NULL);
    if( ret < 0)
        throw Error( Chary("Can't open the file ")+ file_name);
//    std::cout << "Message: '"<<file_name << "' opened for reading." << std::endl;

    ///  Read pkts of a media file to get stream information.
    if(avformat_find_stream_info(fc, NULL)<0)
        throw Error("Couldn't find the stream information.");
//    std::cout << "Message: Found stream information." <<std::endl;

    /// Show the details of opened file
    //av_dump_format(fc, 0, file_name, 0);
}

IFormatContext::~IFormatContext() {
    if(fc)
        avformat_close_input(&fc);
}



OFormatContext::OFormatContext(const Chary& file_name) :FormatContext(),video_frame_count(0),audio_frame_count(0) {
    int ret = avformat_alloc_output_context2(&fc, NULL, NULL, file_name);
    if(ret < 0) {
//        std::cout << "Warning: Couldn't deduce the output format from '"<<file_name << "'" <<std::endl;
        ret = avformat_alloc_output_context2(&fc, NULL, "flv", file_name);
        if(ret < 0)
            throw Error("Couldn't allocte the OutputFormatCotext.");
//        else
//            std::cout << "Notice: The default format flv is used." << std::endl;
    }
//    else
//        std::cout << "Message: '"<<file_name<<"' opened for writing." <<std::endl;

}

OFormatContext::~OFormatContext() {

    if (!(fc->oformat->flags & AVFMT_NOFILE) && fc->pb) {
        av_write_trailer(fc);
        std::cout << "T" << std::endl;
        avio_close(fc->pb);
    }

}

void OFormatContext::openFile() throw(const Error&) {

    if (!(fc->oformat->flags & AVFMT_NOFILE)) {

        if(fc->nb_streams <= 0)
            throw Error("No streams found for muxing.");

        int ret = avio_open(&fc->pb, fc->filename, AVIO_FLAG_WRITE);
        if(ret < 0)
            throw Error(Chary("Could't open the file ")+fc->filename);
        else {
            ret = avformat_write_header(fc,NULL);
            if(ret < 0)
                throw Error("Couldn't write the header.");

            std::cout << "H";
        }
    } else
        throw Error("Couldn't open file. Unexpected Error.");

}


void OFormatContext::writeRawPacket(Packet& pkt) throw(const Error&, const Flush&) {
    if(pkt > 0) {
        int ret = av_interleaved_write_frame(fc, pkt);
        if(ret < 0)
            throw Error("Couldn't write Frame");
    } else {
        // Flushes the data buffer
        av_interleaved_write_frame(fc, NULL);
        throw Flush("Packet");
    }
}

void OFormatContext::writePacket(const CodecContext& codec_context,Packet& pkt) throw(const Error&, const Flush&) {
    if(pkt > 0) {
        /// Rescale output pkt timestamp values from codec to stream timebase
        int pts = av_rescale_q_rnd(pkt.pts(), codec_context.timeBase(), codec_context.stream().timeBase(), static_cast<AVRounding>(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        int dts = av_rescale_q_rnd(pkt.dts(), codec_context.timeBase(), codec_context.stream().timeBase(), static_cast<AVRounding>(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        int duration = av_rescale_q(pkt.duration(), codec_context.timeBase(), codec_context.stream().timeBase());
        int stream_index = codec_context.stream().index();
        pkt.set(pts,dts,duration,stream_index);
    }
    writeRawPacket(pkt);
}


int OFormatContext::writeFrame(const VideoCodecContext& codec_context,VideoFrame& frame) try {

    // The changes in frame is applied here incase user has forgotten to do that manually.
    try{
        frame.apply();
    } catch (const Error& e){
        //std::cout << e << std::endl;
    }

    if( frame &&  (frame.width() < codec_context.width() || frame.height() < codec_context.height() )  ) {
        std::cout << "Warning: The frame size is smaller than the output size." << std::endl;
        std::cout << codec_context.width() <<"x"<< codec_context.height() << "\t" << frame.width() <<"x"<< frame.height() <<std::endl;
    }


    if (fc->oformat->flags & AVFMT_RAWPICTURE) {

        // Incase of rawvideo, directly stores the picture in the packet as no decoding is required.
        Packet pkt;
        if(frame)
            pkt.set(sizeof(AVPicture),frame.data()[0],codec_context.stream().index(),AV_PKT_FLAG_KEY);
        writeRawPacket(pkt);
        std::cout << 'v' ;
        video_frame_count++;

    } else {

         // The pts of frame must be set here as the frame is known to
        // Work 100% only at this point, Oh no I might be wrong
        // Increse the audio framea count to calcualte a proper dts

        Packet pkt;
        frame.setPts(video_frame_count);

        try {
            frame.encoder(codec_context, pkt);
        } catch(const Flush& f) {
            std::cout << '*';
        }

        writePacket(codec_context,pkt);
        std::cout << 'v' ;
        video_frame_count++;
    }

    return 0;

} catch(const Flush& f){
    std::cout << '*';
    return -1;
} catch(const Error& err) {
    std::cout << '?';
    return -2;
}



int OFormatContext::writeFrame(const AudioCodecContext& codec_context,AudioFrame& frame) try {

    Packet pkt;
    frame.setPts(av_rescale_q(audio_frame_count, AVRational{1, codec_context.sampleRate()}, codec_context.timeBase()));
    try {
        frame.encoder(codec_context, pkt);
    } catch(const Flush& f) {
        std::cout << '*';
    }
    writePacket(codec_context,pkt);
    audio_frame_count += frame.nbSamples();
    std::cout << 'a' ;

    return 0;
} catch(const Flush& f){
    std::cout << '*';
    return -1;
}
catch(const Error& err) {
    std::cout << '?';
    return -2;
}
