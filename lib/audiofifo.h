#ifndef AUDIOFIFO_H
#define AUDIOFIFO_H

#include "common.h"

class AudioFIFO {
private:
    AVAudioFifo* fifo;

    AudioFIFO(const AudioFIFO&) = delete;

    AudioFIFO() = delete;

public:
    /// Initialize a FIFO buffer based on the specified output sample format.
    AudioFIFO(const AudioCodecContext& output_codec_context);

    /// Destroys the FIFO buffer
    ~AudioFIFO();

    /// Add converted input audio samples to the FIFO buffer for later processing.
    void put(const AudioFrame& frame) throw(const Error&, const Empty&);

    /// Retrieve output audio samples from the FIFO buffer for processing.
    void get(const AudioFrame& frame) throw(const Error&, const Empty&);

    /// Get the size of the FIFO buffer
    int size() const ;

    operator AVAudioFifo*() const;

};

inline AudioFIFO::operator AVAudioFifo*() const {
    return fifo;
}

inline int AudioFIFO::size() const {
    return av_audio_fifo_size(fifo);
}

#endif // AUDIOFIFO_H
