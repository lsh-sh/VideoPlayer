//
// Created by 刘帅帅 on 2019/12/8.
//

#include "CallJava.h"

CallJava::CallJava(JavaVM *jvm, JNIEnv *env, jobject obj) {
    mJvm = jvm;
    mEnv = env;
    mObject = obj;
    jclass clzz = env->GetObjectClass(obj);
    if (!clzz) {
        LOGE("get object class is failed");
        return;
    }
    methodId = env->GetMethodID(clzz, "onParpared", "()V");
}

CallJava::~CallJava() {

}

void CallJava::onParpared(int threadType) {
    if (threadType == MAIN_THREAD) {
        mEnv->CallVoidMethod(mObject,methodId);
    } else {
        JNIEnv * env;

        if( mJvm->AttachCurrentThread(&env,0) != JNI_OK){
            LOGE("get child jniEnv is failed");
            return;
        }
        env->CallVoidMethod(mObject,methodId);
        mJvm->DetachCurrentThread();
    }
}
