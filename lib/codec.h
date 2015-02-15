#ifndef CODEC_H
#define CODEC_H

#include "common.h"

class Codec {
protected:
    AVCodec* c;

    /// Find the encoder using the codec id
    void getEncoder(AVCodecID id) throw(const Error& );

    /// Find the decoder using the codec id
    void getDecoder(AVCodecID id) throw(const Error& );

    /// Initializes codec to NULL
    Codec();

public:
    Codec(const Codec&) = delete;

    virtual ~Codec() = 0;

    /// Returns the type information.
    AVMediaType type() const;

    /// Returns the type name
    const char* typeName() const;

    /// Returns the codec id. AVCodecID
    AVCodecID id() const;

    /// Returns the name of the codec
    const char* name() const;

    operator AVCodec*() const;
};

class AudioCodec:public Codec {
public:
    AudioCodec(const AudioCodec& ) = delete;

    AudioCodec() = delete;

    AudioCodec(const IFormatContext& fc);

    AudioCodec(const OFormatContext& fc);

    virtual ~AudioCodec();

    /// Returns the supported sample format.
    AVSampleFormat  sampleFormats() const;

    /// Returns the supported sample rates.
    int sampleRates() const;
};

class VideoCodec:public Codec {
public:
    VideoCodec(const VideoCodec&) = delete;

    VideoCodec() = delete;

    VideoCodec(const IFormatContext& fc);

    VideoCodec(const OFormatContext& fc);

    virtual ~VideoCodec();

    /// Returns the supported pixel format.
    AVPixelFormat  pixelFormats() const;

    /// Returns the supported frame rates.
    AVRational frameRates() const;
};

inline Codec::operator AVCodec*() const {
    return c;
}

inline AVCodecID Codec::id() const {
    return c->id;
}

inline AVMediaType Codec::type() const {
    return c->type;
}

inline const char* Codec::typeName() const {
    if(c)
        return av_get_media_type_string(c->type);
    return "None";
}

inline const char* Codec::name() const {
    if (c)
        return avcodec_get_name(c->id);
    return "None";
}


inline AVSampleFormat  AudioCodec::sampleFormats() const {
    if( &(c->sample_fmts[0]) != NULL)
        return c->sample_fmts[0];
    return AV_SAMPLE_FMT_NONE;
}

inline int AudioCodec::sampleRates() const {
    if( &(c->supported_samplerates[0]) != NULL)
        return c->supported_samplerates[0];
    return 44100;
}

inline AVPixelFormat  VideoCodec::pixelFormats() const {
    if ( &(c->pix_fmts[0]) != NULL)
        return c->pix_fmts[0];
    return AV_PIX_FMT_NONE;
}

inline AVRational VideoCodec::frameRates() const {
    if( &(c->supported_framerates[0]) != NULL)
        return c->supported_framerates[0];
    return AVRational{1,25};
}

#endif // CODEC_H
