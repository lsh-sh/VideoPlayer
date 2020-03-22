//
// Created by 刘帅帅 on 2019/12/8.
//

#include "Audio.h"

Audio::Audio(PlayerStatus *playerStatus, int sampleRate) {
    this->playerStatus = playerStatus;
    this->queue = new PacketQueue(playerStatus);
    this->avPacket = av_packet_alloc();
    this->avFrame = av_frame_alloc();
    this->outBuffer = (uint8_t *) av_malloc(sampleRate * 2 * 2);
}

Audio::~Audio() {
    if (queue != NULL) {
        delete (queue);
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
}

int Audio::resampleAudio() {
    SwrContext *swrContext = NULL;
    int dataSize = 0;
    while (playerStatus != NULL && !playerStatus->exit) {

        if (playerStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }

        if (queue->getSize() == 0) {
            playerStatus->load = true;
            playerStatus->playerListner->postEvent(EVNET_LOAD_STAT);
        }

        if (queue->getPacket(avPacket) != 0) {
            av_packet_unref(avPacket);
            continue;
        }

        if (playerStatus->load) {
            playerStatus->load = false;
            playerStatus->playerListner->postEvent(EVNET_LOAD_END);
        }

        int resCode = avcodec_send_packet(codecContext, avPacket);
        if (resCode != 0) {
            av_packet_unref(avPacket);
            continue;
        }
        resCode = avcodec_receive_frame(codecContext, avFrame);
        if (resCode != 0) {
            av_packet_unref(avPacket);
            av_frame_unref(avFrame);
            continue;
        }

        if (avFrame->channels > 0 && avFrame->channel_layout == 0) {
            avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
        } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
            avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
        }

        if (swrContext == NULL) {
            swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                            avFrame->sample_rate, avFrame->channel_layout,
                                            (AVSampleFormat) (avFrame->format),
                                            avFrame->sample_rate, NULL, NULL);
        }
        if (!swrContext || swr_init(swrContext) < 0) {
            if (swrContext != NULL) {
                swr_free(&swrContext);
            }
            swrContext = NULL;
            av_packet_unref(avPacket);
            av_frame_unref(avFrame);
            continue;
        }

        int nb = swr_convert(swrContext, &outBuffer, avFrame->nb_samples,
                             (const uint8_t **) avFrame->data,
                             avFrame->nb_samples);

        int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        dataSize = nb * outChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

        nowTime = avFrame->pts * av_q2d(timeBase);

        if (nowTime < clock) {
            //校验防止avFrame->pts为0
            nowTime = clock;
        }
        clock = nowTime;

        av_packet_unref(avPacket);
        av_frame_unref(avFrame);
        break;
    }
    swr_free(&swrContext);
    return dataSize;
}

void *Audio::decodPlay(void *data) {
    Audio *audio = (Audio *) data;

    audio->initOpenSLES();

    pthread_exit(&audio->threadPlay);
}

void Audio::play() {
    pthread_create(&threadPlay, NULL, Audio::decodPlay, this);
}

//音频数据的回调
void playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    Audio *audio = (Audio *) context;
    if (audio != NULL) {
        int buffersize = audio->resampleAudio();
        if (buffersize > 0) {
            audio->clock += buffersize / ((double) (audio->sampleRate * 2 * 2));
            (*audio->playerBufferQueue)->Enqueue(audio->playerBufferQueue,
                                                 (uint8_t *) audio->outBuffer,
                                                 buffersize);

            if (audio->clock - audio->lastClock >= 1.0) {
                audio->lastClock = audio->clock;
                audio->playerStatus->playerListner->postEvent(EVNET_UPDATE_TIME, audio->duration,
                                                              audio->clock);
            }
        }
    }
}


void Audio::initOpenSLES() {
    SLresult result;
    //第一步---------------------------------------
    //创建引擎对象
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //第二步---------------------------------------
    //创建混音器
    const SLInterfaceID mixIds[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mixReq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mixIds, mixReq);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }

    //第三步---------------------------------------
    //创建播放器
    SLDataLocator_AndroidSimpleBufferQueue bufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                          2};
    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM, //播放pcm格式数据
            2, //2个声道(立体声)
            getSampleRateForOpensles(sampleRate), //44100HZ的频率
            SL_PCMSAMPLEFORMAT_FIXED_16, //位数16位
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN //结束标志
    };
    SLDataSource dataSource = {&bufferQueue, &formatPcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink dataSink = {&outputMix, NULL};

    const SLInterfaceID playIds[2] = {SL_IID_BUFFERQUEUE, SL_IID_PLAYBACKRATE};
    const SLboolean palyReq[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &dataSource, &dataSink,
                                                2, playIds, palyReq);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;
    // 初始化播放器
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;
    //得到接口后调用  获取Player接口
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //第四步---------------------------------------
    // 创建缓冲区和回调函数
    result = (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE,
                                           &playerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;
    ///缓冲接口回调
    result = (*playerBufferQueue)->RegisterCallback(playerBufferQueue, playerCallback, this);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //第五步---------------------------------------
    // 设置播放状态
    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    //第六步---------------------------------------
    // 主动调用回调函数开始工作
    playerCallback(playerBufferQueue, this);
}

SLuint32 Audio::getSampleRateForOpensles(int sampleRate) {
    SLuint32 rate = 0;
    switch (sampleRate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void Audio::pause() {
    if (playerPlay != NULL) {
        (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void Audio::resume() {
    if (playerPlay != NULL) {
        (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void Audio::stop() {
    if (playerPlay != NULL) {
        (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_STOPPED);
    }
}

void Audio::release() {
    stop();
    if (playerObject != NULL) {
        (*playerObject)->Destroy(playerObject);
        playerObject = NULL;
        playerPlay = NULL;
        playerBufferQueue = NULL;
    }
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    if (queue != NULL) {
        delete queue;
        queue = NULL;
    }
    if (outBuffer != NULL) {
        av_free(outBuffer);
        outBuffer = NULL;
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
    playerStatus = NULL;
}


