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

    if(queue != NULL){
        delete  queue;
        queue = NULL;
    }

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

    if (codecpar != NULL) {
        avcodec_parameters_free(&codecpar);
        codecpar = NULL;
    }
    if (codecContext != NULL) {
        avcodec_close(codecContext);
        avcodec_free_context(&codecContext);
        codecContext = NULL;
    }
}

void Video::play() {
    pthread_create(&threadPlay, NULL, decodPlay, this);
}

void *Video::decodPlay(void *data) {
    Video *video = (Video *) data;
    while (video->playerStatus != NULL && !video->playerStatus->exit) {
        if (video->playerStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }

        if (video->queue->getSize() == 0) {
            video->playerStatus->load = true;
            video->playerStatus->playerListner->postEvent(EVNET_LOAD_STAT);
        }

        if (video->queue->getPacket(video->avPacket) != 0) {
            av_packet_unref(video->avPacket);
            continue;
        }

        if (video->playerStatus->load) {
            video->playerStatus->load = false;
            video->playerStatus->playerListner->postEvent(EVNET_LOAD_END);
        }

        int resCode = avcodec_send_packet(video->codecContext, video->avPacket);
        if (resCode != 0) {
            av_packet_unref(video->avPacket);
            continue;
        }
        resCode = avcodec_receive_frame(video->codecContext, video->avFrame);
        if (resCode != 0) {
            av_packet_unref(video->avPacket);
            av_frame_unref(video->avFrame);
            continue;
        }



    }
    pthread_exit(NULL);
}
