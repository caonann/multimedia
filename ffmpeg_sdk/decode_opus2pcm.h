#include <version.h>
#include <cstdio>
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
}

#include <cstdlib>
#include <cstring>

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096