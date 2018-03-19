//
// Created by licht on 2018/3/19.
//
//封装格式
#include "libavformat/avformat.h"
//解码
#include "libavcodec/avcodec.h"
#include "com_zero_licht_ffmpeg_video_utils.h"
#include <android/log.h>
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO ,"zyl",__VA_ARGS__);
JNIEXPORT void JNICALL Java_com_zero_licht_ffmpeg_video_1utils_like
        (JNIEnv * jniEnv, jclass jclass, jstring in, jstring out){
  const char *inPath =   (*jniEnv)->GetStringUTFChars(jniEnv,in,NULL);
    const char *outPath =   (*jniEnv)->GetStringUTFChars(jniEnv,out,NULL);
   //1.注册
    av_register_all();


    //2.打开流
    AVFormatContext *avFormatContext =avformat_alloc_context();

   int a =  avformat_open_input(avFormatContext,inPath,NULL,NULL);
    if (a !=0){
        LOGI("meidakai");
        return;
    }
    //3.湖区文件信息
    if (avformat_find_stream_info(avFormatContext,NULL)<0){
        LOGI("获取视频信息失败");
        return;

    }

    //视频解码，需要找到视频对应的AvStram   avFormatContext->streams所在的索引位置
    int i=0;
    int video_stream_idx= -1;
    for (;i<avFormatContext->nb_streams;i++){
        //根据类型判断，是否是视频流
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_idx = i;
            break;
        }
    }
    //解码器

//    avFormatContext->streams[video_stream_idx].

    (*jniEnv)->ReleaseStringUTFChars(jniEnv,in,inPath);
    (*jniEnv)->ReleaseStringUTFChars(jniEnv,out,outPath);
}
