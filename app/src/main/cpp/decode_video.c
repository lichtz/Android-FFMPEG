//
// Created by licht on 2018/3/21.
//

#include "com_zero_licht_ffmpeg_video_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>

#include "libavformat/avformat.h"
//解码
#include "libavcodec/avcodec.h"

//缩放
#include "libswscale/swscale.h"


#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO ,"zyl",__VA_ARGS__);
#define INBUF_SIZE 4096



JNIEXPORT void JNICALL Java_com_zero_licht_ffmpeg_video_1utils_like
        (JNIEnv * jniEnv, jclass jclass, jstring in, jstring out) {
    //需要回收
    const char *inPath = (*jniEnv)->GetStringUTFChars(jniEnv, in, NULL);
    const char *outPath = (*jniEnv)->GetStringUTFChars(jniEnv, out, NULL);
     //注册组建
    av_register_all();
    //打开封装  ，取得封装的上下文
     //保存整个视频的编码器解码器 信息 码率帧率。。
    AVFormatContext  *avFormatContext =  avformat_alloc_context();
    LOGI("zyl wwwwww%s\n",inPath);

    int avformat_open_inout_result =  avformat_open_input(&avFormatContext,inPath,NULL,NULL);
    if (avformat_open_inout_result !=0){
        LOGI("zyl 打开文件失败");
        return;
    }
    //查找视频流
   int find_stream_result =   avformat_find_stream_info(avFormatContext,NULL);
    if (find_stream_result<0){
        LOGI("zyl","查找视频流失败");
    }

    //拿到视频解码器
    int  video_index = -1;
    int i =0;
    for(;i<avFormatContext->nb_streams;i++){
        if ((avFormatContext->streams[i]->codecpar->codec_type)==AVMEDIA_TYPE_VIDEO){
             video_index = i;
            break;
        }
    }
    //根据视频流拿到解码器


    AVCodecParameters * avCodecParameters =  avFormatContext->streams[video_index]->codecpar;
   AVCodec  *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);

//    AVCodecParserContext *avCodecParserContext = av_parser_init(avCodec->id);

    AVCodecContext * c = avcodec_alloc_context3(avCodec);
    int codec_open_result =  avcodec_open2(c,avCodec,NULL);

     //读取视频压缩数据 so AVPacket
    AVPacket * avPacket =  av_packet_alloc();
    AVFrame *  avFrame=  av_frame_alloc();



    LOGI("zyl  %d   %s\n",codec_open_result,avCodec->name);
    (*jniEnv)->ReleaseStringUTFChars(jniEnv,in,inPath);
    (*jniEnv)->ReleaseStringUTFChars(jniEnv,out,outPath);


}