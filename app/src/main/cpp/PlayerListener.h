//
// Created by 刘帅帅 on 2020/3/7.
//

#ifndef VIDEOPLAYER_PLAYERLISTENER_H
#define VIDEOPLAYER_PLAYERLISTENER_H

#include <jni.h>

enum {
    EVNET_LOAD_STAT = 1,
    EVNET_LOAD_END = 2,
    EVNET_PARPARED = 3,
    EVNET_COMPLETE = 4,
    EVNET_UPDATE_TIME = 5
};

class PlayerListener {
private:
    jobject mObject;
    jclass mClass;
public:
    PlayerListener(jobject player, jobject weak_obj);

    ~PlayerListener();

    void postEvent(int what);

    void postEvent(int what, int arg1);

    void postEvent(int what, int arg1, int arg2);

    void postEvent(int what, int arg1, int arg2, void *obj);
};


#endif //VIDEOPLAYER_PLAYERLISTENER_H
