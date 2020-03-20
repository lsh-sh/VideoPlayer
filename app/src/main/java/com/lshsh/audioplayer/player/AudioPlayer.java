package com.lshsh.audioplayer.player;

import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.NonNull;

import com.lshsh.audioplayer.bean.Time;
import com.lshsh.audioplayer.listener.OnCompleteListener;
import com.lshsh.audioplayer.listener.OnLoadListener;
import com.lshsh.audioplayer.listener.OnParparedListener;
import com.lshsh.audioplayer.listener.OnTimeListener;

import java.lang.ref.WeakReference;

public class AudioPlayer {

    private static final String TAG = "AudioPlayer";

    private OnLoadListener onLoadListener;
    private OnParparedListener onParparedListener;
    private OnTimeListener onTimeListener;
    private OnCompleteListener onCompleteListener;
    private String source;
    private EventHandler eventHandler;

    static {
        System.loadLibrary("avcodec-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("swresample-2");
        System.loadLibrary("postproc-54");
        System.loadLibrary("avutil-55");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("audioplayer");
        n_init();
    }

    public AudioPlayer() {
        eventHandler = new EventHandler(this);
        n_setup(new WeakReference<>(this));
    }

    private native void n_setup(Object weak_obj);


    private static native void n_init();

    /**
     * 接收从jni层传递过来的消息
     *
     * @param what
     * @param arg1
     * @param arg2
     */
    private static void MessageFromNative(Object object, int what, int arg1, int arg2) {
        AudioPlayer audioPlayer = ((WeakReference<AudioPlayer>) object).get();
        if (audioPlayer != null) {
            Message message = audioPlayer.eventHandler.obtainMessage(what, arg1, arg2);
            audioPlayer.eventHandler.sendMessage(message);
        }
    }

    public void setSource(String source) {
        this.source = source;
    }

    public void parpared() {
        if (TextUtils.isEmpty(source)) {
            Log.d(TAG, "parpared: source is empty");
            return;
        }
        native_parpared(source);
    }

    public void start() {
        if (TextUtils.isEmpty(source)) {
            Log.d(TAG, "parpared: source is empty");
            return;
        }
        native_start();
    }

    public void pause() {
        Log.d(TAG, "暂停中");
        native_pause();
    }

    public void resume() {
        Log.d(TAG, "播放中");
        native_resume();
    }

    public void stop() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                native_stop();
            }
        }).start();
    }

    public void seek(int secds) {
        native_seek(secds);
    }

    public void setOnLoadListener(OnLoadListener onLoadListener) {
        this.onLoadListener = onLoadListener;
    }

    public void setOnParparedListener(OnParparedListener onParparedListener) {
        this.onParparedListener = onParparedListener;
    }

    public void setOnTimeListener(OnTimeListener onTimeListener) {
        this.onTimeListener = onTimeListener;
    }

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
    }

    private native void native_parpared(String source);

    private native void native_start();

    private native void native_pause();

    private native void native_resume();

    private native void native_stop();

    private native void native_seek(int secds);


    private static class EventHandler extends Handler {
        private WeakReference<AudioPlayer> weakReference;
        private static final int EVNET_LOAD_STAT = 1;
        private static final int EVNET_LOAD_END = 2;
        private static final int EVNET_PARPARED = 3;
        private static final int EVNET_COMPLETE = 4;
        private static final int EVNET_UPDATE_TIME = 5;

        public EventHandler(AudioPlayer audioPlayer) {
            weakReference = new WeakReference<>(audioPlayer);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            AudioPlayer audioPlayer = weakReference.get();
            switch (msg.what) {
                case EVNET_LOAD_STAT:
                case EVNET_LOAD_END:
                    if (audioPlayer.onLoadListener != null) {
                        audioPlayer.onLoadListener.onLoad(msg.what == EVNET_LOAD_STAT);
                    }
                    break;
                case EVNET_PARPARED:
                    if (audioPlayer.onParparedListener != null) {
                        audioPlayer.onParparedListener.onParpared();
                    }
                    break;
                case EVNET_COMPLETE:
                    audioPlayer.stop();
                    if (audioPlayer.onCompleteListener != null) {
                        audioPlayer.onCompleteListener.onComplete();
                    }
                    break;
                case EVNET_UPDATE_TIME:
                    if (audioPlayer.onTimeListener != null) {
                        audioPlayer.onTimeListener.onTime(msg.arg1, msg.arg2);
                    }
                    break;
            }
        }
    }
}
