#ifndef COMMON_H
#define COMMON_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#include "libavutil/audio_fifo.h"
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
}

#include <sys/stat.h>
#include <iostream>
#include <queue>
#include "maths.h"
#include "chary.h"
#include "error.h"

// Forward Declarations
class Packet;
class FormatContext;
class IFormatContext;
class OFormatContext;
class Codec;
class AudioCodec;
class VideoCodec;
class Frame;
class Stream;
class VideoStream;
class VideoFrame;
class AudioStream;
class AudioFrame;
class CodecContext;
class VideoCodecContext;
class AudioCodecContext;
class ResampleContext;
class AudioFIFO;


 inline bool fileExists (const Chary& name) {
	struct stat buffer;
	return (stat (name, &buffer) == 0);
}


#endif // COMMON_H
