package com.lshsh.videoplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.lshsh.audioplayer.listener.OnLoadListener;
import com.lshsh.audioplayer.listener.OnParparedListener;
import com.lshsh.audioplayer.player.AudioPlayer;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "AudioPlayer";
    AudioPlayer audioPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        audioPlayer = new AudioPlayer();
        audioPlayer.setOnParparedListener(new OnParparedListener() {
            @Override
            public void onParpared() {
                Log.i(TAG, "onParpared: 准备好了，可以播放了");
                audioPlayer.start();
            }
        });
        audioPlayer.setOnLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean load) {
                if (load) {
                    Log.i(TAG, "onLoad: 加载中");
                } else {
                    Log.i(TAG, "onLoad: 播放中");
                }
            }
        });
    }

    public void begin(View view) {
        audioPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        audioPlayer.parpared();
    }

    public void pause(View view) {
        audioPlayer.pause();
    }

    public void resume(View view) {
        audioPlayer.resume();
    }
}
