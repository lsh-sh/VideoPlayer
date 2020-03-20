//
// Created by 刘帅帅 on 2019/12/8.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue(PlayerStatus *playerStatus) {
    this->playerStatus = playerStatus;
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

PacketQueue::~PacketQueue() {
    clearPacketQueue();
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int PacketQueue::putPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    queuePacket.push(packet);
//    LOGI("放入一个AVPacket到队列里了，现在有%d个", queuePacket.size());
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int PacketQueue::getPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    if (queuePacket.size() > 0) {
        while (playerStatus != NULL && !playerStatus->exit) {
            AVPacket *avPacket = queuePacket.front();
            if (av_packet_ref(packet, avPacket) == 0) {
                queuePacket.pop();
            }

//            LOGI("从队列里取出一个AVPacket，还剩%d个", queuePacket.size());
            av_packet_unref(avPacket);
            av_free(avPacket);
            break;
        }
    } else {
        pthread_cond_wait(&condPacket, &mutexPacket);
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int PacketQueue::getSize() {
    int count = 0;
    pthread_mutex_lock(&mutexPacket);
    count = queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);
    return count;
}

void PacketQueue::clearPacketQueue() {
    //先解除线程锁
    pthread_cond_signal(&condPacket);
    //在加锁
    pthread_mutex_lock(&mutexPacket);
    while (!queuePacket.empty()) {
        AVPacket *packet = queuePacket.front();
        queuePacket.pop();
        av_packet_unref(packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);
}
