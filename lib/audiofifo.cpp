#include "common.h"
#include "frame.h"
#include "codeccontext.h"
#include "audiofifo.h"

AudioFIFO::AudioFIFO(const AudioCodecContext& output_codec_context):fifo(NULL) {
    fifo = av_audio_fifo_alloc(output_codec_context.sampleFormat(), output_codec_context.channels(), 1);
    if (!fifo)
        throw Error("Couldn't allocate FIFO buffer.");
}

AudioFIFO::~AudioFIFO() {
    if(fifo)
        av_audio_fifo_free(fifo);
}

void AudioFIFO::put(const AudioFrame& frame) throw(const Error&, const Empty&) {

    if(!frame)
        throw Empty("Couldn't write data to FIFO buffer.","Frame");

    /// Make FIFO buffer large enough to hold both old and new samples
    int ret = av_audio_fifo_realloc(fifo, size() + frame.nbSamples());
    if (ret < 0)
        throw Error("Couldn't reallocate FIFO buffer.");

    /// Store the new samples in the FIFO buffer.
    ret = av_audio_fifo_write(fifo, reinterpret_cast<void **>(frame.extendedData()),frame.nbSamples());
    if ( ret < frame.nbSamples())
        throw Error("Couldn't write data to FIFO buffer.");

}

void AudioFIFO::get(const AudioFrame& frame) throw(const Error&, const Empty&) {

    if(!frame)
        throw Empty("Couldn't get frame from FIFO buffer.","Frame");

    int ret = av_audio_fifo_read(fifo, reinterpret_cast<void **>(frame.extendedData()), frame.nbSamples());
    if (ret < frame.nbSamples())
        throw Error("Couldn't read data from FIFO buffer.");
}
