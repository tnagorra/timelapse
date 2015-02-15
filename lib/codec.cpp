#include "common.h"
#include "formatcontext.h"
#include "codec.h"


void Codec::getEncoder(AVCodecID id) throw(const Error& ) {
    if(id == AV_CODEC_ID_NONE)
        throw Error("Codec id is None.");
    c = avcodec_find_encoder(id);
    if(!c)
        throw Error( Chary("Couldn't find the encoder for ")+name() );
    //  std::cout << "Message: Found the encoder for '" << name() << "'" << std::endl;
}

void Codec::getDecoder(AVCodecID id) throw(const Error& ) {
    if(id == AV_CODEC_ID_NONE)
        throw Error("Codec id is None.");
    c = avcodec_find_decoder(id);
    if(!c)
        throw Error( Chary("Couldn't find the decoder for ")+name() );
    //   std::cout << "Message: Found the decoder for '" << name() << "'" << std::endl;
}

Codec::Codec():c(NULL) {}

Codec::~Codec() {}

AudioCodec::AudioCodec(const IFormatContext& fc) :Codec() {
    getDecoder( fc.audioCodecId() );
}

AudioCodec::AudioCodec(const OFormatContext& fc) :Codec() {
    getEncoder( fc.audioCodecId() );
}

AudioCodec::~AudioCodec() {}


VideoCodec::VideoCodec(const IFormatContext& fc) :Codec() {
    getDecoder( fc.videoCodecId() );
}

VideoCodec::VideoCodec(const OFormatContext& fc) :Codec() {
    getEncoder( fc.videoCodecId() );
}

VideoCodec::~VideoCodec() {}

