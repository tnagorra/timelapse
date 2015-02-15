#ifndef CODECCONTEXT_H
#define CODECCONTEXT_H

#include "common.h"


class CodecContext {
protected:
    AVCodecContext* cc;

    Stream& s;

    /// Initializes the codec context using the codec
    void initialize() throw(const Error&);

     /// Get codec context data from the stream
    CodecContext(Stream& s);

public:

    CodecContext(const CodecContext&) = delete;

    CodecContext() = delete;

    /// Destroys the CodecContext
    virtual ~CodecContext() =0;

    /// Returns the type base of the codec context
    AVRational timeBase() const;

    /// Returns the codec type of the codec context
    AVMediaType codecType() const;

    operator AVCodecContext*() const;

    const Stream& stream() const;
};


class VideoCodecContext:public CodecContext {
private:
    /// Set the parameters of video codec context
    void set(const OFormatContext& fc,int w,int h,int b);

public:
    VideoCodecContext(const VideoCodec&) = delete;

    VideoCodecContext() = delete;

    /// Initialize the video codec context
    VideoCodecContext(const IFormatContext& fc);

    /// Initialize the video codec context and set the required parameters
    VideoCodecContext(const OFormatContext& fc,int w=0,int h= 0,int b = 5000000);  // Some formats want stream headers to be separate. Checks that

    virtual ~VideoCodecContext();

    /// Returns the width of the video codec context
    int width() const;

    /// Return the height of the video codec context
    int height() const;

    float frameRate() const;
//    AVPixelFormat pixelFormat() const;

};

class AudioCodecContext:public CodecContext {
private:
    /// Set the parameters of audio codec context
    void set(const OFormatContext& fc,int b);

public:
    AudioCodecContext(const AudioCodec&) = delete;

    AudioCodecContext() = delete;

    /// Initialize the audio codec context
    AudioCodecContext(const IFormatContext& fc);

    /// Initialize the audio codec context and set the required parametes
    AudioCodecContext(const OFormatContext& fc,int b = 64000);       // Checks header from this passed fc

    virtual ~AudioCodecContext();

    /// Returns no of audio channels
    int channels() const;

    /// Returns the channel layout
    uint64_t channelLayout() const;

    /// Retuns a supported sample format
    AVSampleFormat sampleFormat() const ;

    /// Returns a supported sample rate
    int sampleRate()const;

    /// Returns the no. of samples per channel
    int frameSize()const;

    int capabilities() const;
};



/// Returns the AVCodecContext*
inline CodecContext::operator AVCodecContext*() const {
    return cc;
}

/// For both types of CodecContext
inline AVRational CodecContext::timeBase() const {
    return cc->time_base;
}

inline AVMediaType CodecContext::codecType() const {
    return cc->codec_type;
}


inline int VideoCodecContext::width() const {
    return cc->width;
}

inline int VideoCodecContext::height() const {
    return cc->height;
}

inline float VideoCodecContext::frameRate() const {
    return cc->time_base.den / cc->time_base.num;
}

inline int AudioCodecContext::channels() const {
    return cc->channels;
}

inline uint64_t AudioCodecContext::channelLayout() const {
    return cc->channel_layout;
}

inline AVSampleFormat AudioCodecContext::sampleFormat() const {
    return cc->sample_fmt;
}

inline int AudioCodecContext::sampleRate()const {

    return cc->sample_rate;
}

inline int AudioCodecContext::frameSize()const {
    return cc->frame_size;
}

inline int AudioCodecContext::capabilities() const {
    return cc->codec->capabilities;
}

#endif // CODECCONTEXT_H
