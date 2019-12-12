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

    int streamIndex = av_find_best_stream(audioPlayer->formatContext, AVMEDIA_TYPE_AUDIO, -1, -1,
                                          NULL, 0);
    AVCodecParameters *codecpar = audioPlayer->formatContext->streams[streamIndex]->codecpar;
    Audio *audio = audioPlayer->audio;
    if (audio == NULL) {
        audio = new Audio(audioPlayer->playerStatus, codecpar->sample_rate, audioPlayer->callJava);
        audioPlayer->audio = audio;
    }
    audio->streamIndex = streamIndex;
    audio->codecpar = codecpar;

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
    audio->play();
    while (audioPlayer->playerStatus != NULL && !audioPlayer->playerStatus->exit) {
        AVPacket *packet = av_packet_alloc();
        if (av_read_frame(audioPlayer->formatContext, packet) >= 0) {
            audio->queue->putPacket(packet);
        } else {
            av_packet_unref(packet);
            av_free(packet);
            while (audioPlayer->playerStatus != NULL && !audioPlayer->playerStatus->exit) {
                if (audio->queue->getSize() > 0) {
                    continue;
                } else {
                    audio->playerStatus->exit = true;
                    break;
                }
            }
        }
    }
    LOGI("解码完成");
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

AudioPlayer::AudioPlayer(CallJava *callJava, const char *url, PlayerStatus *playerStatus) {
    this->playerStatus = playerStatus;
    this->callJava = callJava;
    this->url = new char[strlen(url)];
    strcpy(this->url, url);
}

void AudioPlayer::start() {
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}

void AudioPlayer::pause() {
    if (audio != NULL) {
        audio->pause();
    }
}

void AudioPlayer::resume() {
    if (audio != NULL) {
        audio->resume();
    }
}
