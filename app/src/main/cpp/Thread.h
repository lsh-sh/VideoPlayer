//
// Created by 刘帅帅 on 2020/3/18.
//

#ifndef VIDEOPLAYER_THREAD_H
#define VIDEOPLAYER_THREAD_H

//#include <pthread.h>

class Thread {
private:
//    pthread_t pthread;
public:
    void run(void *(*func)(void *), void *args);

    ~Thread();
};

inline void Thread::run(void *(*func)(void *), void *args) {
//    pthread_create(&pthread, NULL, func, args);
}

Thread::~Thread() {
//    pthread_exit(&pthread);
}

#endif //VIDEOPLAYER_THREAD_H
