package com.zero.licht.ffmpeg;

/**
 * Created by licht on 2018/3/19.
 */

public class video_utils {
    public  static  native  void like (String in,String out);
    static {

            System.loadLibrary("avcodec-57");
            System.loadLibrary("avdevice-57");
            System.loadLibrary("avfilter-6");
            System.loadLibrary("avformat-57");
            System.loadLibrary("avutil-55");
            System.loadLibrary("postproc-54");
            System.loadLibrary("swresample-2");
            System.loadLibrary("swscale-4");
            System.loadLibrary("native-lib");

    }
}
