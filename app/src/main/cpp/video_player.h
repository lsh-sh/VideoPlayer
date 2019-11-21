//
// Created by 刘帅帅 on 2019/11/20.
//

#ifndef NDKAPPLICATION_VIDEO_PLAYER_H
#define NDKAPPLICATION_VIDEO_PLAYER_H

#include "ffmpeg/include/libavformat/avformat.h"
#include "ffmpeg/include/libavcodec/avcodec.h"
#include "ffmpeg/include/libswscale/swscale.h"
#include <stdlib.h>
#include "common.h"
AVFormatContext* formatContext;
void init();
int open(const char* videoPath, const char* outPath);
#endif //NDKAPPLICATION_VIDEO_PLAYER_H
