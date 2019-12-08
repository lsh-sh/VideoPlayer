//
// Created by 刘帅帅 on 2019/12/8.
//

#ifndef VIDEOPLAYER_ANDROIDLOG_H
#define VIDEOPLAYER_ANDROIDLOG_H
#include <android/log.h>
#define TAG "AudioPlayer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#endif //VIDEOPLAYER_ANDROIDLOG_H
