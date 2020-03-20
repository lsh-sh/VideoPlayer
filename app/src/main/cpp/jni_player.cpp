//
// Created by 刘帅帅 on 2019/12/8.
//
#include <jni.h>
#include "AndroidLog.h"
#include "PlayerListener.h"
#include "MediaPlayer.h"
#include "PlayerStatus.h"

static JavaVM *javaVm = NULL;
jmethodID post_event = NULL;
MediaPlayer *audioPlayer = NULL;
PlayerStatus *playerStatus = NULL;
PlayerListener *playerListener = NULL;
bool nativeExit = false;
extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    javaVm = vm;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_4;
}

static JNIEnv *getEnv() {
    assert(javaVm != NULL);
    JNIEnv *env = NULL;
    if (javaVm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return NULL;
    }
    return env;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1parpared(JNIEnv *env, jobject thiz,
                                                               jstring _source) {
    const char *source = env->GetStringUTFChars(_source, 0);
    LOGI("url is %s", source);
    if (playerStatus == NULL) {
        playerStatus = new PlayerStatus();
        playerStatus->setPlayerListener(playerListener);
    }
    if (audioPlayer == NULL) {
        audioPlayer = new MediaPlayer(source, playerStatus);
    }
    playerListener->postEvent(EVNET_LOAD_STAT);
    audioPlayer->parpared();
    env->ReleaseStringUTFChars(_source, source);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1start(JNIEnv *env, jobject thiz) {
    if (audioPlayer != NULL) {
        audioPlayer->start();
    }
    nativeExit = false;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1pause(JNIEnv *env, jobject thiz) {
    if (audioPlayer != NULL) {
        audioPlayer->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1resume(JNIEnv *env, jobject thiz) {
    if (audioPlayer != NULL) {
        audioPlayer->resume();
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1stop(JNIEnv *env, jobject thiz) {

    if (nativeExit) {
        return;
    }

    if (audioPlayer != NULL) {
        audioPlayer->stop();
        audioPlayer->release();
        delete (audioPlayer);
        audioPlayer = NULL;
    }
    if (playerStatus != NULL) {
        delete (playerStatus);
        playerStatus = NULL;
    }
    nativeExit = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_native_1seek(JNIEnv *env, jobject thiz, jint secds) {
    if (audioPlayer != NULL) {
        audioPlayer->seek(secds);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_n_1init(JNIEnv *env, jclass clazz) {
    post_event = env->GetStaticMethodID(clazz, "MessageFromNative",
                                        "(Ljava/lang/Object;III)V");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lshsh_audioplayer_player_AudioPlayer_n_1setup(JNIEnv *env, jobject thiz,
                                                       jobject weak_obj) {
    playerListener = new PlayerListener(thiz, weak_obj);
}


PlayerListener::PlayerListener(jobject player, jobject weak_obj) {
    mObject = NULL;
    mClass = NULL;
    JNIEnv *env = getEnv();
    mObject = env->NewGlobalRef(weak_obj);
    jclass clzz = env->GetObjectClass(player);
    mClass = (jclass) env->NewGlobalRef(clzz);
    env->DeleteLocalRef(clzz);
}

PlayerListener::~PlayerListener() {
    JNIEnv *env = getEnv();
    if (mObject != NULL) {
        env->DeleteGlobalRef(mObject);
    }
    if (mClass != NULL) {
        env->DeleteGlobalRef(mClass);
    }
}

void PlayerListener::postEvent(int what) {
    postEvent(what, 0);
}

void PlayerListener::postEvent(int what, int arg1) {
    postEvent(what, arg1, 0);
}

void PlayerListener::postEvent(int what, int arg1, int arg2) {
    postEvent(what, arg1, arg2, NULL);
}

void PlayerListener::postEvent(int what, int arg1, int arg2, void *obj) {
    JNIEnv *env = getEnv();
    bool status = false;
    if (env == NULL) {
        //当前线程没有env，调用AttachCurrentThread 附加当前线程到虚拟机VM当中
        status = (javaVm->AttachCurrentThread(&env, NULL) >= 0);
    }

    env->CallStaticVoidMethod(mClass, post_event, mObject, what, arg1, arg2);
    if (env->ExceptionCheck()) {
        LOGE("An exception occurred while notifying an event.");
        env->ExceptionClear();
    }
    if (status) {
        javaVm->DetachCurrentThread();
    }
}
