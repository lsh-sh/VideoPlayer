//
// Created by 刘帅帅 on 2019/12/8.
//

#ifndef VIDEOPLAYER_MEDIAPLAYER_H
#define VIDEOPLAYER_MEDIAPLAYER_H
extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libavutil/time.h"
}

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "Audio.h"
#include "Video.h"
#include "PlayerStatus.h"


class MediaPlayer {
public:
    char *url = NULL;
    pthread_t initThread;
    pthread_t decodeThread;
    AVFormatContext *formatContext = NULL;
    Audio *audio = NULL;
    Video *video = NULL;
    PlayerStatus *playerStatus = NULL;
    pthread_mutex_t mutex;
    pthread_mutex_t seekMutex;
    bool exit = false;
public:
    MediaPlayer(const char *url, PlayerStatus *playerStatus);

    ~MediaPlayer();

    void parpared();

    void start();

    void pause();

    void resume();

    static void *decodeFFmpeg(void *data);

    static void *initFFmpeg(void *data);


    void stop();

    void seek(int64_t secds);

    void release();

    int getCodeContext(AVCodecParameters *codecpar, AVCodecContext *&codecContext);
};


#endif //VIDEOPLAYER_MEDIAPLAYER_H
