//
// Created by 刘帅帅 on 2019/12/8.
//

#ifndef VIDEOPLAYER_AUDIOPLAYER_H
#define VIDEOPLAYER_AUDIOPLAYER_H
extern "C" {
#include "include/libavformat/avformat.h"
}
#include <string.h>
#include <pthread.h>
#include "CallJava.h"
#include "Audio.h"
#include "PlayerStatus.h"
class AudioPlayer {
public:
    CallJava* callJava = NULL;
    char * url = NULL;
    pthread_t initThread;
    pthread_t decodeThread;
    AVFormatContext* formatContext = NULL;
    Audio* audio = NULL;
    PlayerStatus* playerStatus = NULL;
public:
    AudioPlayer(CallJava* callJava, const char* url,PlayerStatus* playerStatus);

    ~AudioPlayer();

    void parpared();

    void start();

    void pause();

    void resume();
};


#endif //VIDEOPLAYER_AUDIOPLAYER_H
