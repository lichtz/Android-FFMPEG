package com.zero.licht.ffmpeg;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
          tv.setOnClickListener(new View.OnClickListener() {
              @Override
              public void onClick(View v) {

                  video_utils.like(Environment.getExternalStorageDirectory()+ File.separator+"with"
                  +File.separator+"l.mp4",Environment.getExternalStorageDirectory()+ File.separator+"with"
                          +File.separator+"o.mp4");
              }
          });
    }


}
