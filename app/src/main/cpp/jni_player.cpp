//
// Created by 刘帅帅 on 2019/12/8.
//
#include <jni.h>
#include "AndroidLog.h"
#include "CallJava.h"
#include "AudioPlayer.h"

JavaVM *javaVm = NULL;
CallJava *callJava = NULL;
jobject objParparedListener;
AudioPlayer *audioPlayer = NULL;
extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    javaVm = vm;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_4;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_setOnParparedListener(JNIEnv *env, jobject thiz,
                                                                    jobject on_parpared_listener) {
    objParparedListener = env->NewGlobalRef(on_parpared_listener);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1parpared(JNIEnv *env, jobject thiz,
                                                               jstring _source) {
    const char *source = env->GetStringUTFChars(_source, 0);
    if (callJava == NULL) {
        callJava = new CallJava(javaVm, env, objParparedListener);
    }
    LOGI("url is %s", source);
    if (audioPlayer == NULL) {
        audioPlayer = new AudioPlayer(callJava, source);
    }
    audioPlayer->parpared();
    env->ReleaseStringUTFChars(_source, source);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1start(JNIEnv *env, jobject thiz) {
        if(audioPlayer != NULL){
            audioPlayer->start();
        }
}