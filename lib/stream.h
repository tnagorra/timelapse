#ifndef STREAM_H
#define STREAM_H
#include "codec.h"
#include "common.h"

class Stream {
protected:
    AVStream *s;

    Codec& cp;

    /// Get stream data from the format context
    void getStream(const IFormatContext& fc,int id) throw(const Error&);

    /// Allocate a new stream and add it to the format context
    void putStream(const OFormatContext& fc) throw(const Error&);

    /// Initialize a stream with a codec
    Stream(Codec& c);

public:

    Stream(const Stream&) = delete;

    Stream() = delete;

    virtual ~Stream() =0;

    /// Returns the index number of the stream
    int index() const;

    /// Returns the time base of the stream
    AVRational timeBase() const;

    /// Returns the current time in the stream
    double time() const ;

    operator AVStream*() const;

    AVCodecContext* codecContext() const;

   virtual  const Codec& codec() const = 0;

};

class AudioStream:public Stream {
public:
    AudioStream(const AudioStream&) = delete;

    AudioStream() = delete;

    AudioStream(const IFormatContext& fc);

    AudioStream(const OFormatContext& fc);

    virtual ~AudioStream();

    const  AudioCodec& codec() const;

};

class VideoStream:public Stream {
public:
    VideoStream(const VideoStream&) = delete;

    VideoStream() = delete;

    VideoStream(const IFormatContext& fc);

    VideoStream(const OFormatContext& fc);

    virtual ~VideoStream();

    const VideoCodec& codec() const;

};


inline Stream::operator AVStream*() const {
    return s;
}

inline AVCodecContext* Stream::codecContext() const {
    return s->codec;
}

inline int Stream::index() const {
    return s->index;
}

inline AVRational Stream::timeBase() const {
    return s->time_base;
}

inline double Stream::time() const {
    return  s->pts.val * av_q2d(s->time_base);
}

#endif // STREAM_H
