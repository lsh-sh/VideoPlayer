//
// Created by 刘帅帅 on 2020/3/20.
//

#include "Video.h"

Video::Video(PlayerStatus *playerStatus) {
    this->playerStatus = playerStatus;
    this->queue = new PacketQueue(playerStatus);
    this->avFrame = av_frame_alloc();
    this->avPacket = av_packet_alloc();
}

Video::~Video() {
    if (avPacket != NULL) {
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }
    if (avFrame != NULL) {
        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;
    }
}

void Video::play() {
    pthread_create(&threadPlay, NULL, decodPlay, this);
}

void *Video::decodPlay(void *data) {
    Video *video = (Video *) data;
    while (video->playerStatus != NULL && !video->playerStatus->exit) {
        if(video->queue->getPacket(video->avPacket) == 0){
            LOGI("从队列中拿取一帧");
        }
    }
    pthread_exit(NULL);
}
