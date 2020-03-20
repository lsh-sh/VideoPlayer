//
// Created by 刘帅帅 on 2019/12/8.
//

#ifndef VIDEOPLAYER_AUDIO_H
#define VIDEOPLAYER_AUDIO_H
extern "C" {
#include "include/libavcodec/avcodec.h"
#include "include/libswresample/swresample.h"
#include "include/libavutil/time.h"
};

#include "PlayerStatus.h"
#include "PacketQueue.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class Audio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext *codecContext = NULL;
    PlayerStatus *playerStatus = NULL;
    PacketQueue *queue = NULL;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    uint8_t *outBuffer = NULL;
    pthread_t threadPlay;
    int sampleRate = 44100;
    int duration = 0;
    double nowTime = 0;
    double clock = 0;
    double lastClock = 0;
    AVRational timeBase;

    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;
    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    SLObjectItf playerObject = NULL;
    SLPlayItf playerPlay = NULL;
    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf playerBufferQueue;
public:
    Audio(PlayerStatus *playerStatus, int sampleRate);

    ~Audio();

    void play();

    static void *decodPlay(void *);

    int resampleAudio();

    void initOpenSLES();

    SLuint32 getSampleRateForOpensles(int sampleRate);

    void pause();

    void resume();

    void stop();

    void release();
};


#endif //VIDEOPLAYER_AUDIO_H
