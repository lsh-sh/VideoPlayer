//
// Created by 刘帅帅 on 2019/12/8.
//

#include "CallJava.h"

CallJava::CallJava(JavaVM *jvm, JNIEnv *env) {
    mJvm = jvm;
    mEnv = env;
}

CallJava::~CallJava() {

}

void CallJava::onParpared(int threadType) {
    if (threadType == MAIN_THREAD) {
        mEnv->CallVoidMethod(onParparedObject, jmidOnParpared);
    } else {
        JNIEnv *env;

        if (mJvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child jniEnv is failed");
            return;
        }
        env->CallVoidMethod(onParparedObject, jmidOnParpared);
        mJvm->DetachCurrentThread();
    }
}

void CallJava::setOnParparedObject(jobject obj) {
    jclass clzz = mEnv->GetObjectClass(obj);
    onParparedObject = obj;
    if (!clzz) {
        LOGE("get object class is failed");
        return;
    }
    jmidOnParpared = mEnv->GetMethodID(clzz, "onParpared", "()V");
}

void CallJava::onLoad(int threadType, bool load) {
    if (threadType == MAIN_THREAD) {
        mEnv->CallVoidMethod(onLoadObject, jmidOnLoad, load);
    } else {
        JNIEnv *env;

        if (mJvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child jniEnv is failed");
            return;
        }
        env->CallVoidMethod(onLoadObject, jmidOnLoad, load);
        mJvm->DetachCurrentThread();
    }
}

void CallJava::setOnLoadObject(jobject obj) {
    jclass clzz = mEnv->GetObjectClass(obj);
    onLoadObject = obj;
    if (!clzz) {
        LOGE("get object class is failed");
        return;
    }
    jmidOnLoad = mEnv->GetMethodID(clzz, "onLoad", "(Z)V");
}
