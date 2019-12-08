//
// Created by 刘帅帅 on 2019/12/8.
//

#ifndef VIDEOPLAYER_CALLJAVA_H
#define VIDEOPLAYER_CALLJAVA_H

#include "AndroidLog.h"
#include <jni.h>
#define MAIN_THREAD 0
#define CHILD_THREAD 1
class CallJava {
private:
    JavaVM *mJvm;
    JNIEnv* mEnv;
    jobject  mObject;
    jmethodID methodId;
public:
    CallJava(JavaVM *jvm,JNIEnv* env,jobject obj);
    ~CallJava();
    void onParpared(int threadType);
};


#endif //VIDEOPLAYER_CALLJAVA_H
