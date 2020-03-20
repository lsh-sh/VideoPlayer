//
// Created by 刘帅帅 on 2019/12/8.
//

#include "MediaPlayer.h"

int avformatCallback(void *ctx) {
    MediaPlayer *mediaPlayer = (MediaPlayer *) ctx;
    if (mediaPlayer->playerStatus->exit) {
        return AVERROR_EOF;
    }
    return 0;
}


int MediaPlayer::getCodeContext(AVCodecParameters *codecpar, AVCodecContext *&codecContext) {
    AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        LOGE("can not find decoder");
        return -1;
    }

    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        LOGE("can not new codecContext");
        return -1;
    }
    if (avcodec_parameters_to_context(codecContext, codecpar) < 0) {
        LOGE("can not fill codecContext");
        return -1;
    }

    if (avcodec_open2(codecContext, codec, NULL) != 0) {
        LOGE("can not open codec");
        return -1;
    }

    return 0;
}

void *MediaPlayer::initFFmpeg(void *data) {
    MediaPlayer *mediaPlayer = (MediaPlayer *) data;
    pthread_mutex_lock(&mediaPlayer->mutex);
    av_register_all();
    //初始化网络
    avformat_network_init();

    mediaPlayer->formatContext = avformat_alloc_context();
    mediaPlayer->formatContext->interrupt_callback.callback = avformatCallback;
    mediaPlayer->formatContext->interrupt_callback.opaque = mediaPlayer;
    if (avformat_open_input(&mediaPlayer->formatContext, mediaPlayer->url, NULL, NULL) != 0) {

        LOGE("can not open url %s", mediaPlayer->url);
        mediaPlayer->exit = true;
        pthread_mutex_unlock(&mediaPlayer->mutex);
        return 0;
    }

    if (avformat_find_stream_info(mediaPlayer->formatContext, NULL) < 0) {
        LOGE("can not find stream info");
        mediaPlayer->exit = true;
        pthread_mutex_unlock(&mediaPlayer->mutex);
        return 0;
    }

    //初始化音频解码数据
    int audioStreamIndex = av_find_best_stream(mediaPlayer->formatContext, AVMEDIA_TYPE_AUDIO, -1,
                                               -1,
                                               NULL, 0);
    AVCodecParameters *audioCodecpar = mediaPlayer->formatContext->streams[audioStreamIndex]->codecpar;
    if (mediaPlayer->audio == NULL) {
        mediaPlayer->audio = new Audio(mediaPlayer->playerStatus, audioCodecpar->sample_rate);
        mediaPlayer->audio->streamIndex = audioStreamIndex;
        mediaPlayer->audio->codecpar = audioCodecpar;
        mediaPlayer->audio->duration = mediaPlayer->formatContext->duration / AV_TIME_BASE;
        mediaPlayer->audio->timeBase = mediaPlayer->formatContext->streams[audioStreamIndex]->time_base;
    }


    //初始化视频解码数据
    int videoStreamIndex = av_find_best_stream(mediaPlayer->formatContext, AVMEDIA_TYPE_VIDEO, -1,
                                               -1, NULL, 0);
    AVCodecParameters *videoCodecpar = mediaPlayer->formatContext->streams[videoStreamIndex]->codecpar;
    if (mediaPlayer->video == NULL) {
        mediaPlayer->video = new Video(mediaPlayer->playerStatus);
        mediaPlayer->video->streamIndex = videoStreamIndex;
        mediaPlayer->video->codecpar = videoCodecpar;
        mediaPlayer->video->timeBase = mediaPlayer->formatContext->streams[videoStreamIndex]->time_base;
    }

    if (mediaPlayer->audio != NULL &&
        mediaPlayer->getCodeContext(audioCodecpar, mediaPlayer->audio->codecContext) == -1) {
        mediaPlayer->exit = true;
        pthread_mutex_unlock(&mediaPlayer->mutex);
        return 0;
    }

    if (mediaPlayer->video != NULL &&
        mediaPlayer->getCodeContext(videoCodecpar, mediaPlayer->video->codecContext) == -1) {
        mediaPlayer->exit = true;
        pthread_mutex_unlock(&mediaPlayer->mutex);
        return 0;
    }

    mediaPlayer->playerStatus->playerListner->postEvent(EVNET_PARPARED);
    pthread_mutex_unlock(&mediaPlayer->mutex);
    pthread_exit(&mediaPlayer->initThread);
}

void *MediaPlayer::decodeFFmpeg(void *data) {
    MediaPlayer *mediaPlayer = (MediaPlayer *) data;
    Audio *audio = mediaPlayer->audio;
    Video *video = mediaPlayer->video;
    if (audio == NULL) {
        LOGE("audio is null");
        return 0;
    }
    audio->play();
    video->play();

    while (mediaPlayer->playerStatus != NULL && !mediaPlayer->playerStatus->exit) {
        if (mediaPlayer->playerStatus->seek) {
            av_usleep(1000 * 100);  //睡眠100毫秒
            continue;
        }
        if (audio->queue->getSize() > 40) {  //队列缓存40帧
            av_usleep(1000 * 100);  //睡眠100毫秒
            continue;
        }
        AVPacket *packet = av_packet_alloc();
        pthread_mutex_lock(&mediaPlayer->seekMutex);
        int reslut = av_read_frame(mediaPlayer->formatContext, packet);
        pthread_mutex_unlock(&mediaPlayer->seekMutex);
        if (reslut >= 0) {
            //获取到一个packet
            if (packet->stream_index == audio->streamIndex) {
                audio->queue->putPacket(packet);
            }
            if (packet->stream_index == video->streamIndex) {
                video->queue->putPacket(packet);
            }
        } else {
            //没有取到packet，有可能出错也有可能到达文件末尾
            av_packet_unref(packet);
            av_free(packet);
            if (reslut == AVERROR_EOF) {
                break;
            }
        }
    }
    mediaPlayer->playerStatus->playerListner->postEvent(EVNET_COMPLETE);
    mediaPlayer->exit = true;
    pthread_exit(&mediaPlayer->decodeThread);
}

void MediaPlayer::parpared() {
    pthread_create(&this->initThread, NULL, MediaPlayer::initFFmpeg, this);
}

MediaPlayer::~MediaPlayer() {
    if (this->url != NULL) {
        delete this->url;
    }
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&seekMutex);
}

MediaPlayer::MediaPlayer(const char *url, PlayerStatus *playerStatus) {
    this->playerStatus = playerStatus;
    this->url = av_strdup(url);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&seekMutex, NULL);
}

void MediaPlayer::start() {
    pthread_create(&decodeThread, NULL, MediaPlayer::decodeFFmpeg, this);
}

void MediaPlayer::pause() {
    if (audio != NULL) {
        audio->pause();
    }
}

void MediaPlayer::resume() {
    if (audio != NULL) {
        audio->resume();
    }
}

void MediaPlayer::release() {
    playerStatus->exit = true;

    pthread_mutex_lock(&mutex);
    int seelpCount = 0;
    while (!exit) {
        if (seelpCount > 1000) {
            exit = true;
        }
        seelpCount++;
        LOGI("等待退出 %d", seelpCount);
        usleep(1000 * 10);
    }

    if (audio != NULL) {
        audio->release();
        delete (audio);
        audio = NULL;
    }
    if (formatContext != NULL) {
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
        formatContext = NULL;
    }
    playerStatus = NULL;
    pthread_mutex_unlock(&mutex);
}

void MediaPlayer::stop() {
    if (audio != NULL) {
        audio->stop();
    }
}

void MediaPlayer::seek(int64_t secds) {
    if (audio == NULL || audio->duration <= 0) {
        return;
    }

    if (secds >= 0 && secds <= audio->duration) {
        playerStatus->seek = true;
        audio->queue->clearPacketQueue();
        audio->clock = 0;
        audio->lastClock = 0;
        pthread_mutex_lock(&seekMutex);
        int64_t rel = secds * AV_TIME_BASE;
        avformat_seek_file(formatContext, -1, INT64_MIN, rel, INT64_MAX, 0);
        pthread_mutex_unlock(&seekMutex);
        playerStatus->seek = false;

    }

}
