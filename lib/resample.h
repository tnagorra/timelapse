#ifndef RESAMPLE_H
#define RESAMPLE_H

#include "common.h"

class ResampleContext {
private:
    SwrContext* resample_context;

public:
	ResampleContext(const ResampleContext&) = delete;

    ResampleContext() = delete;

    /// Initialize the audio resampler based on the input and output codec settings.
    ResampleContext(const AudioCodecContext& input_codec_context,const AudioCodecContext& output_codec_context);

    /// Destroys the resample context
    ~ResampleContext();

    operator SwrContext*() const;
};

inline ResampleContext::operator SwrContext*() const {
    return resample_context;
}


#endif // RESAMPLE_H
