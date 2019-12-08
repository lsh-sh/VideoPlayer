//
// Created by 刘帅帅 on 2019/12/8.
//

#ifndef VIDEOPLAYER_AUDIO_H
#define VIDEOPLAYER_AUDIO_H
extern "C"{
#include "include/libavcodec/avcodec.h"
};
class Audio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext * codecContext = NULL;
};


#endif //VIDEOPLAYER_AUDIO_H
