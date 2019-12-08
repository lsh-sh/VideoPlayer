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
class PacketQueue {
public:
    std::queue<AVPacket*> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
public:
    PacketQueue();
    ~PacketQueue();
    int putPacket(AVPacket* packet);
    int getPacket(AVPacket* packet);
};


#endif //VIDEOPLAYER_PACKETQUEUE_H
