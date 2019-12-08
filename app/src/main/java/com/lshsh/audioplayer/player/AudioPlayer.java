package com.lshsh.audioplayer.player;

import android.text.TextUtils;
import android.util.Log;

import com.lshsh.audioplayer.listener.OnParparedListener;

import org.w3c.dom.Text;

public class AudioPlayer {

    private static final String TAG = "AudioPlayer";
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
    }

    private String source;

    public void setSource(String source) {
        this.source = source;
    }

    public void parpared(){
        if(TextUtils.isEmpty(source)){
            Log.d(TAG, "parpared: source is empty");
            return;
        }
        native_parpared(source);
    }

    public void start() {
        if(TextUtils.isEmpty(source)){
            Log.d(TAG, "parpared: source is empty");
            return;
        }
        native_start();
    }

    public native void setOnParparedListener(OnParparedListener onParparedListener);

    private native void native_parpared(String source);

    private native void native_start();
}
