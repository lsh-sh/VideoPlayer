package com.lshsh.videoplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

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
                audioPlayer.start();
            }
        });
    }

    public void begin(View view) {
        audioPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        audioPlayer.parpared();
    }
}
