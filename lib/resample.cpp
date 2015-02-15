#include "common.h"
#include "resample.h"
#include "codeccontext.h"

ResampleContext::ResampleContext(const AudioCodecContext& input_codec_context,const AudioCodecContext& output_codec_context) :resample_context(NULL) {

    /// Perform a check so that no. of converted samples isn't greater
    /// than the no. of samples to be converted. Sample rates shoudln't differs
    if(output_codec_context.sampleRate() != input_codec_context.sampleRate() )
        throw Error("The sample rates don't match");

    /// Create a resampler context for the conversion.
    resample_context = swr_alloc_set_opts(NULL,
                                          av_get_default_channel_layout(output_codec_context.channels()),
                                          output_codec_context.sampleFormat(),
                                          output_codec_context.sampleRate(),
                                          av_get_default_channel_layout(input_codec_context.channels()),
                                          input_codec_context.sampleFormat(),
                                          input_codec_context.sampleRate(),
                                          0, NULL);
    if (!resample_context)
        throw Error("Couldn't allocate ResampleContext.");

    /// Open the resampler with the specified parameters.
    int ret = swr_init(resample_context);
    if (ret < 0)
        throw Error("Couldn't initialize ResampleContext.");
}


ResampleContext::~ResampleContext() {
    if(resample_context)
        swr_free(&resample_context);
}

