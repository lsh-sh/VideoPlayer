//
// Created by 刘帅帅 on 2019/12/8.
//

#ifndef VIDEOPLAYER_PACKETQUEUE_H
#define VIDEOPLAYER_PACKETQUEUE_H
extern "C"{
#include "include/libavcodec/avcodec.h"
};

#include "AndroidLog.h"
#include <queue>
#include <pthread.h>
#include "PlayerStatus.h"
class PacketQueue {
public:
    std::queue<AVPacket*> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    PlayerStatus* playerStatus = NULL;
public:
    PacketQueue(PlayerStatus* playerStatus);
    ~PacketQueue();
    int putPacket(AVPacket* packet);
    int getPacket(AVPacket* packet);
    int getSize();
};


#endif //VIDEOPLAYER_PACKETQUEUE_H
