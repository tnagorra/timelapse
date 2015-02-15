#include "common.h"
#include "codec.h"
#include "stream.h"
#include "formatcontext.h"
#include "codeccontext.h"


void CodecContext::initialize() throw(const Error&) {
    /// Initialize the codecContext to use codec
    int ret = avcodec_open2(cc, s.codec() , NULL);
    if(ret < 0)
        throw Error( Chary("Couldn't initialize CodecContext for ")+s.codec().typeName() );
    //std::cout << "Message: CodecContext opened for " << codec.typeName() << std::endl;
}

CodecContext::CodecContext(Stream& s):cc(NULL),s(s) {
    cc = s.codecContext();
}

/// Closes the CodecContext
CodecContext::~CodecContext() {
    if(cc)
        avcodec_close(cc);
    delete &s;
}

const Stream& CodecContext::stream() const{
    return s;
}



void VideoCodecContext::set(const OFormatContext& fc,int w,int h,int b) {

    const VideoCodec& codec = dynamic_cast<const VideoCodec&>(s.codec());

    h = h > 0 ? h : 0;
    w = w > 0 ? w : 0;
    cc->codec_id = codec.id();
    cc->bit_rate = b;
    cc->width    = Maths::evenize(w);
    cc->height   = Maths::evenize(h);
    cc->time_base = codec.frameRates();
    cc->gop_size      = 12;
    cc->pix_fmt       = codec.pixelFormats();
//    cc->rc_max_rate = 9000000; //+ 10000;
//    cc->rc_min_rate = 9000000; //- 10000;
//    cc->rc_buffer_size = 9000000*100;
//    cc->rc_max_available_vbv_use = 9000000*100;
//    cc->me_method       = 5;
//    cc->compression_level = 0;
    if (cc->codec_id == AV_CODEC_ID_MPEG2VIDEO)
        cc->max_b_frames = 2;							// just for testing, we also add B frames
    else if (cc->codec_id == AV_CODEC_ID_MPEG1VIDEO)
        cc->mb_decision = 2;

    if (fc.outputFormat() != NULL && (fc.outputFormat()->flags & AVFMT_GLOBALHEADER)) // Some formats want stream headers to be separate.
        cc->flags |= CODEC_FLAG_GLOBAL_HEADER;

}

VideoCodecContext::VideoCodecContext(const IFormatContext& fc)  :CodecContext( *new VideoStream(fc) ) {
    initialize();
}

VideoCodecContext::VideoCodecContext(const OFormatContext& fc,int w,int h,int b) :CodecContext( *new VideoStream(fc)) {
    set(fc,w,h,b);
    initialize();
}

VideoCodecContext::~VideoCodecContext() {}


void AudioCodecContext::set(const OFormatContext& fc,int b) {

    const AudioCodec& codec = dynamic_cast<const AudioCodec&>(s.codec());

    cc->sample_fmt  = codec.sampleFormats();
    cc->bit_rate    = b;//64,96,128,160,192
    cc->sample_rate = codec.sampleRates();//*codec.sampleRates();;
    cc->channels    = 2;
    cc->channel_layout = av_get_default_channel_layout(cc->channels);

    if (fc.outputFormat()->flags & AVFMT_GLOBALHEADER)     // Some formats want stream headers to be separate.
        cc->flags |= CODEC_FLAG_GLOBAL_HEADER;
}


AudioCodecContext::AudioCodecContext(const IFormatContext& fc):CodecContext(*new AudioStream(fc)) {
    initialize();
}

AudioCodecContext::AudioCodecContext(const OFormatContext& fc,int b):CodecContext(*new AudioStream(fc)) {
    set(fc,b);
    initialize();
}

AudioCodecContext::~AudioCodecContext() {}
