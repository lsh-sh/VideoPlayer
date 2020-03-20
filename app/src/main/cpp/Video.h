//
// Created by 刘帅帅 on 2020/3/20.
//

#ifndef VIDEOPLAYER_VIDEO_H
#define VIDEOPLAYER_VIDEO_H
extern "C" {
#include "include/libavcodec/avcodec.h"
};

#include "PlayerStatus.h"
#include "PacketQueue.h"

class Video {
public:
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext *codecContext = NULL;
    PlayerStatus *playerStatus = NULL;
    PacketQueue *queue = NULL;
    pthread_t threadPlay;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    AVRational timeBase;
public:
    Video(PlayerStatus *playerStatus);

    static void* decodPlay(void * data);
    ~Video();

    void play();
};


#endif //VIDEOPLAYER_VIDEO_H
