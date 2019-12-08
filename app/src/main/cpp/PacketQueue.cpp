//
// Created by 刘帅帅 on 2019/12/8.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue() {
    pthread_mutex_init(&mutexPacket,NULL);
    pthread_cond_init(&condPacket,NULL);
}

PacketQueue::~PacketQueue() {
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int PacketQueue::putPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    queuePacket.push(packet);
    LOGI("放入一个AVPacket到队列里了，现在有%d个",queuePacket.size());
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int PacketQueue::getPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}
