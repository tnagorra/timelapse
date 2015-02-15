#ifndef FRAME_H
#define FRAME_H

#include "common.h"

enum effects{NONE,BW,GRAYSCALE};

class Frame {
protected:
    AVFrame* f;

    bool usable;

    /// Allocate the memory for the frame
    void allocate() throw(const Error&) ;

    /// Sets the end of stream
    virtual void streamEnd(IFormatContext& fc) = 0;

    /// Gets the decoder to read the frame
    virtual int decoder(const CodecContext& cc,Packet& pkt,int& complete)  throw(const Error&,const Flush&,const Empty&) = 0;

    /// Decodes the frame and sets the EOS if format context is depleted
    virtual void readFrame(IFormatContext& fc,const CodecContext& cc, int stream_id) throw(const Error&,const Empty&);

    /// Initializes frame to NULL
    Frame();

public:

    Frame(const Frame&) = delete;

    /// Gets the encoder to write the frame
    virtual void encoder(const CodecContext& cc, Packet& pkt)  throw(const Error&,const Flush&) = 0;

    /// Frees the frame
    virtual ~Frame();

    /// Set the pts of the frame, is used while writing the frame
    void setPts(int t);

    /// Returns the frame data
    uint8_t** data() const;

    /// Returns the extended frame data
    uint8_t** extendedData() const;

    operator bool() const;

    operator AVFrame*() const;

};

class VideoFrame:public Frame {
private:
    // An empty video frame cannot be made.
    // Must be read from input so there is no set function.
    effects effect_v;

    int output_width, output_height,real_width,real_height,pan_x,pan_y,strip_x,strip_y;
    float brightness_v,saturation_v,scale_ratio;
    bool modified;

    /// Sets the end of video stream
    void streamEnd(IFormatContext& fc);

    /// Gets the video decoder
    int decoder(const CodecContext& cc,Packet& pkt,int& complete)  throw(const Error&,const Flush&,const Empty&);

    /// Check for deprecated format types and replace it with an equivalent
    void checkDeprecated(int * format);

public:
    VideoFrame(const VideoFrame&);;

    VideoFrame() = delete;

    /// Allocate the frame and decode from the stream
    /// ALso checks for deprecated formats and converts the colorspace to AV_PIX_FMT_YUV420P
    /// Gets the scale size of the image so as to get the real size required
    /// Gets the optimized image size
    VideoFrame(IFormatContext& fc,const VideoCodecContext& cc,const VideoCodecContext& oc,int x,int y);

    /// Destroys the video frame
    virtual ~VideoFrame();

    /// Gets the video encoder
    void encoder(const CodecContext& cc,Packet& pkt)  throw(const Error&,const Flush&);

    /// Returns the height of the video frame
    int height() const;

    /// Retuns the width of the video frame
    int width() const;

    /// Returns the pixel format of the frame
    AVPixelFormat pixelFormat() const;

    /// Set the starting pixel to read the data from
    void pan(int x=0,int y = 0);

    /// Sets the widht and height of empty space
    void strip(int x=0, int y=0);

    /// Sets the brightness_v of image range from -1 to 1
    void brightness(float b = 0.0);

    /// Sets the saturation_v fo image range from 0 to 1
    void saturation(float s = 1.0);

    /// Apply all the changes made to the image except the scale
    /// All work is done in this function simulataneously to increase efficieny
    void apply()  throw(const Error&,const Empty&);

    /// Scale the image as well change the pixel format of the frame
    void scale( AVPixelFormat pix_fmt = AV_PIX_FMT_NONE, int width=0, int height=0) throw(const Error&,const Empty&);

    void effect(effects e);
};

class AudioFrame:public Frame {
private:

    bool silence;

    /// Sets the end of audio stream
    void streamEnd(IFormatContext& fc);

    /// Gets  the decoder for audio
    int decoder(const CodecContext& cc,Packet& pkt,int& complete) throw(const Error&,const Flush&,const Empty&);

    /// Sets the parameters of audio frame and also get the buffer
    void set(const AudioCodecContext& occ,int nb_samples) throw(const Error&,const Empty&);

    /// Resample the audio frame
    void resample(const ResampleContext& resample_context,const AudioCodecContext& output_codec_context) throw(const Error&,const Empty&);

    AudioFrame(const AudioFrame&);


public:

    AudioFrame();

    /// Gets the encoder for audio
    void encoder(const CodecContext& cc, Packet& pkt) throw(const Error&,const Flush&) ;

    /// Allocate the frame, decode,resample the frame and put it in the fifo buffer unless it is the required size
    /// then delete the frame and allocate it again to get the frame from the buffer
    AudioFrame(IFormatContext& fc,const AudioCodecContext& cc, const AudioCodecContext& occ, const ResampleContext& resample_ctx, AudioFIFO& fifo);

    /// Destroys the audio frame
    virtual ~AudioFrame();

    /// Returns the no of samples
    int nbSamples() const;

};


inline Frame::operator AVFrame*() const {
    return f;
}

inline void Frame::setPts(int t) {
    if(f)
        f->pts = t;
}

inline uint8_t** Frame::data() const {
    if(f)
        return f->data;
    return NULL;
}

inline uint8_t** Frame::extendedData() const {
    if(f)
        return f->extended_data;
    return NULL;
}



inline Frame::operator bool() const {
    return usable;
}

inline void VideoFrame::checkDeprecated(int * format) {

    switch(*format) {
    case  AV_PIX_FMT_YUVJ420P:
        *format = PIX_FMT_YUV420P;
        break;
    case  AV_PIX_FMT_YUVJ422P:
        *format = PIX_FMT_YUV422P;
        break;
    case  AV_PIX_FMT_YUVJ444P:
        *format = PIX_FMT_YUV444P;
        break;
    case  AV_PIX_FMT_YUVJ440P:
        *format = PIX_FMT_YUV440P;
        break;
    case  AV_PIX_FMT_YUVJ411P:
        *format = PIX_FMT_YUV411P;
        break;
    }

}

inline int VideoFrame::height() const {
    if(f)
        return f->height;
    return -1;
}

inline int VideoFrame::width() const {
    if(f)
        return f->width;
    return -1;
}

inline AVPixelFormat VideoFrame::pixelFormat() const{
    return static_cast<AVPixelFormat>(f->format);
}


inline void VideoFrame::pan(int x,int y) {
    if(f) {
        pan_x = Maths::limit(scale_ratio*x,f->width,0);
        pan_y = Maths::limit(scale_ratio*y,f->height,0);
    }
}

inline void VideoFrame::strip(int x, int y) {

    if(output_width && output_height) {
        strip_x = Maths::limit(scale_ratio*x,output_width,0);
        strip_y = Maths::limit(scale_ratio*y,output_height,0);
    }else if(f){
        strip_x = Maths::limit(scale_ratio*x,f->width,0);
        strip_y = Maths::limit(scale_ratio*y,f->height,0);
    }
}

inline void VideoFrame::brightness(float b) {
    brightness_v = Maths::limit(b,1.0,-1.0);
}

inline void VideoFrame::saturation(float s) {
    saturation_v = Maths::limit(s,1.0,0.0);
}

inline int AudioFrame::nbSamples() const {
    if(!f)
        return -1;
    return f->nb_samples;
}

#endif // FRAME_H
