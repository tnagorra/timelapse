#include "common.h"
#include "formatcontext.h"
#include "codec.h"
#include "stream.h"


void Stream::getStream(const IFormatContext& fc,int id) throw(const Error&) {
    s = fc.streams(id);
    if(!s)
        throw Error("Couldn't get Stream for id.");
}

void Stream::putStream(const OFormatContext& fc) throw(const Error&) {
    s = avformat_new_stream(fc,cp);
    if(!s)
        throw Error( Chary("Couldn't put new Stream for ")+ cp.typeName() );
    s->id = fc.totalStreams()-1;
}

Stream::Stream(Codec& c):s(NULL),cp(c) {}

Stream::~Stream() {
    delete &cp;
}

const AudioCodec& AudioStream::codec() const{
    return dynamic_cast<AudioCodec&>(cp);
}

const VideoCodec& VideoStream::codec() const{
    return dynamic_cast<VideoCodec&>(cp);
}


AudioStream::AudioStream(const IFormatContext& fc):Stream(*new AudioCodec(fc)) {
    getStream(fc,fc.videoStreamId());
}

AudioStream::AudioStream(const OFormatContext& fc):Stream(*new AudioCodec(fc)) {
    putStream(fc);
}

AudioStream::~AudioStream(){}

VideoStream::VideoStream(const IFormatContext& fc):Stream( *new VideoCodec(fc)) {
    getStream(fc,fc.audioStreamId());
}

VideoStream::VideoStream(const OFormatContext& fc):Stream(*new VideoCodec(fc)) {
    putStream(fc);
}

VideoStream::~VideoStream(){}
