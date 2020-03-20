package com.lshsh.videoplayer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.lshsh.audioplayer.bean.Time;
import com.lshsh.audioplayer.listener.OnCompleteListener;
import com.lshsh.audioplayer.listener.OnLoadListener;
import com.lshsh.audioplayer.listener.OnParparedListener;
import com.lshsh.audioplayer.listener.OnTimeListener;
import com.lshsh.audioplayer.player.AudioPlayer;
import com.lshsh.audioplayer.util.TimeUtil;

import java.lang.ref.WeakReference;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "AudioPlayer";
    private AudioPlayer audioPlayer;
    private TextView tvTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvTime = findViewById(R.id.tv_time);
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
        audioPlayer.setOnTimeListener(new OnTimeListener() {
            @Override
            public void onTime(int duration, int currentPosition) {
                tvTime.setText(TimeUtil.formatSeconds(currentPosition) + "/" + TimeUtil.formatSeconds(duration));
            }
        });
        audioPlayer.setOnCompleteListener(new OnCompleteListener() {
            @Override
            public void onComplete() {
                Log.i(TAG, "onComplete: 播放完成");
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

    public void stop(View view) {
        audioPlayer.stop();
    }

    public void seek(View view) {
        audioPlayer.seek(200);
    }

}
