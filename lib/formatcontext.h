#ifndef FORMATCONTEXT_H
#define FORMATCONTEXT_H

#include "common.h"


class FormatContext {
protected:
    AVFormatContext* fc;

    /// Initializes the fc to NULL;
    FormatContext();

public:
    FormatContext(const FormatContext&) = delete;

    /// Destroys the fc
    virtual ~FormatContext() = 0;

    /// Returns the total number of streams on the format context.
    int totalStreams() const;

    /// Returns the streams contained in the format context.
    AVStream* streams(unsigned int i) const throw(const Error&);

    /// Returns the audio codec id of the format context
    virtual AVCodecID audioCodecId() const = 0;

    /// Returns the video codec id of the format context
    virtual AVCodecID videoCodecId() const = 0;

    operator AVFormatContext*() const;

};


class IFormatContext:public FormatContext {
private:
    bool audio_complete;
    bool video_complete;

public:
    IFormatContext(const IFormatContext&) = delete;

    IFormatContext() = delete;

    /// Opens the input stream using the filename and read the header.
    /// And find the stream information contained in the file.
    /// Allocates the fc and initializes it.
    IFormatContext(const Chary& file_name);

    /// Close the file and destroys the format context
    virtual ~IFormatContext();

    /// Returns the video codec id from the FormatContext::streams[video_codec_id]->codec_id
    AVCodecID videoCodecId()const;

    /// Returns the audio codec id from the FormatContext::streams[audio_codec_id]->codec_id
    AVCodecID audioCodecId()const;

    /// Returns the video stream id form the FormatContext::video_codec_id
    int videoStreamId() const;

    /// On read mode, returns the audio stream id form the FormatContext::audio_codec_id
    int audioStreamId() const;

    /// Returns if the audio stream is depleted
    bool audioEOS() const;

    /// Returns if the video stream is depleted
    bool videoEOS() const;

    /// Sets the end of audio stream
    void setAudioEOS(bool b = true);

    /// Sets the end of video stream
    void setVideoEOS(bool b = true);

};


class OFormatContext:public FormatContext {
private:
    int video_frame_count;
    int audio_frame_count;

     /// Writes a packet to the format context
    /// A packet will null data is sent to flush the data
    void writeRawPacket(Packet& pkt) throw(const Error&, const Flush&);

    /// Writes data to the format context using the
    void writePacket(const CodecContext& codec_context,Packet& pkt) throw(const Error&, const Flush&);

public:
    OFormatContext(const OFormatContext&) = delete;

    OFormatContext() = delete;

    /// Allocates the format context and guess the format using the filename
    OFormatContext(const Chary& file_name);

    /// Destroys the format context
    virtual ~OFormatContext();

    /// Opens a file to write data.and  writes the header to the file
    /// This function must be called after FormatContext, Stream, Codec and CodecContext are defined
    void openFile() throw(const Error&);

    /// Returns the output format.
    AVOutputFormat* outputFormat() const ;

    /// Returns the video codec id from the FormatContext::oformat->video_codec
    AVCodecID videoCodecId()const;

    /// Returns the audio codec id from the FormatContext::oformat->audio_codec
    AVCodecID audioCodecId() const;

    /// Writes a video frame to the format context
    int writeFrame(const VideoCodecContext& codec_context,VideoFrame& frame);

    /// Writes an audio frame to the format context
    int writeFrame(const AudioCodecContext& codec_context,AudioFrame& frame);

};


inline FormatContext::operator AVFormatContext*() const {
    return fc;
}

inline int FormatContext::totalStreams() const {
    return fc->nb_streams;
}

inline AVCodecID IFormatContext::videoCodecId()const {
    return fc->streams[fc->video_codec_id]->codec->codec_id;
}

inline AVCodecID IFormatContext::audioCodecId()const {
    return fc->streams[fc->audio_codec_id]->codec->codec_id;
}

inline int IFormatContext::videoStreamId() const {
    return fc->video_codec_id;
}

inline int IFormatContext::audioStreamId() const {
    return fc->audio_codec_id;
}

inline bool IFormatContext::audioEOS() const {
    return audio_complete;
}

inline bool IFormatContext::videoEOS() const {
    return video_complete;
}

inline void IFormatContext::setAudioEOS(bool b) {
    audio_complete = b;
}

inline void IFormatContext::setVideoEOS(bool b) {
    video_complete = b;
}

inline AVOutputFormat* OFormatContext::outputFormat() const {
    return fc->oformat;
}

inline AVCodecID OFormatContext::videoCodecId()const {
    return fc->oformat->video_codec;
}

inline AVCodecID OFormatContext::audioCodecId()const {
    return fc->oformat->audio_codec;
}


#endif // FORMATCONTEXT_H

