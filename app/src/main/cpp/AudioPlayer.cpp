//
// Created by 刘帅帅 on 2019/12/8.
//

#include "AudioPlayer.h"

void *initFFmpeg(void *data) {
    AudioPlayer *audioPlayer = (AudioPlayer *) data;
    av_register_all();
    //初始化网络
    avformat_network_init();

    audioPlayer->formatContext = avformat_alloc_context();
    if (avformat_open_input(&audioPlayer->formatContext, audioPlayer->url, NULL, NULL) != 0) {

        LOGE("can not open url %s", audioPlayer->url);
        return 0;
    }

    if (avformat_find_stream_info(audioPlayer->formatContext, NULL) < 0) {
        LOGE("can not find stream info");
        return 0;
    }

    Audio *audio = audioPlayer->audio;
    if (audio == NULL) {
        audio = new Audio();
        audioPlayer->audio = audio;
    }
    audio->streamIndex = av_find_best_stream(audioPlayer->formatContext, AVMEDIA_TYPE_AUDIO, -1, -1,
                                             NULL, 0);
    audio->codecpar = audioPlayer->formatContext->streams[audio->streamIndex]->codecpar;

    AVCodec *codec = avcodec_find_decoder(audio->codecpar->codec_id);
    if (!codec) {
        LOGE("can not find decoder");
        return 0;
    }

    audio->codecContext = avcodec_alloc_context3(codec);
    if (!audio->codecContext) {
        LOGE("can not new codecContext");
        return 0;
    }
    if (avcodec_parameters_to_context(audio->codecContext, audio->codecpar) < 0) {
        LOGE("can not fill codecContext");
        return 0;
    }

    if (avcodec_open2(audio->codecContext, codec, NULL) != 0) {
        LOGE("can not open codec");
        return 0;
    }

    audioPlayer->callJava->onParpared(CHILD_THREAD);
    pthread_exit(&audioPlayer->initThread);
}

void *decodeFFmpeg(void *data) {
    AudioPlayer *audioPlayer = (AudioPlayer *) data;
    Audio *audio = audioPlayer->audio;
    if (audio == NULL) {
        LOGE("audio is null");
        return 0;
    }
    int count = 0;
    AVPacket *packet = av_packet_alloc();
    while (av_read_frame(audioPlayer->formatContext, packet) >= 0) {
        if (packet->stream_index == audio->streamIndex) {
            count++;
            LOGI("解码第 %d 帧", count);
        }
        av_packet_unref(packet);
    }
    av_free(packet);
    packet = NULL;

    pthread_exit(&audioPlayer->decodeThread);
}

void AudioPlayer::parpared() {
    pthread_create(&this->initThread, NULL, initFFmpeg, this);
}

AudioPlayer::~AudioPlayer() {
    if (this->url != NULL) {
        delete this->url;
    }
}

AudioPlayer::AudioPlayer(CallJava *callJava, const char *url) {
    this->callJava = callJava;
    this->url = new char[strlen(url)];
    strcpy(this->url, url);
}

void AudioPlayer::start() {
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}
