//
// Created by licht on 2018/3/19.
//
//封装格式
#include "libavformat/avformat.h"
//解码
#include "libavcodec/avcodec.h"
#include "com_zero_licht_ffmpeg_video_utils.h"
#include <android/log.h>
#include "libswscale/swscale.h"
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
    //解码器,从avstream得到解码器id，根据id 找到解码器。

    AVCodecParameters * avCodecParameters =avFormatContext->streams[video_stream_idx]->codecpar;
    AVCodec *avCodec =  avcodec_find_decoder(avCodecParameters->codec_id);

    if (avCodec == NULL){
        LOGI("无法解码");
        return;
    }
    //打开解码器
    if (avcodec_open2(avFormatContext,avCodec,NULL)<0){
         LOGI("解码器无法打开");
        return;
    }
    //读取眼avPacket 压缩数据（编码数据）
    AVPacket *packet;
    packet = av_packet_alloc();

    //读取
    AVFrame * avFrame;
    avFrame = av_frame_alloc();
    if (!avFrame) {
        fprintf(stderr, "不能申请到fram空间\n");
        return;
    }
    //输出文件
   FILE * fp_yuv =  fopen(outPath,"wb");


    int len =0 ,framecount =0;
   //读取编码的视频文件 packet
    while (av_read_frame(avFormatContext,packet)>=0){
      len  =  avcodec_receive_frame(avFormatContext,avFrame);
        if (len){

//            SwsContext c =sws_getContext(avFrame->width,avFrame->height,avCodec->pix_fmts,
//                           avFrame->width,avFrame->height,  AV_PIX_FMT_YUV420P,
//                           SWS_BILINEAR,NULL,NULL,NULL);
//            sws_scale()

            //正在解码 向yuv文件报错解码之后的数据 以转码未YUV计算
            int y_size = avFrame->width*avFrame->height;

            fwrite(avFrame->data[0],1,y_size,fp_yuv );
            fwrite(avFrame->data[1],1,y_size/4,fp_yuv );
            fwrite(avFrame->data[2],1,y_size/4,fp_yuv );
            LOGI("解码%d \n",framecount++);


        }
        av_packet_unref(packet);

    }
    fclose(fp_yuv);
    av_frame_free(avFrame);
    avcodec_close(avFormatContext);
    avcodec_free_context(avCodec);

    (*jniEnv)->ReleaseStringUTFChars(jniEnv,in,inPath);
    (*jniEnv)->ReleaseStringUTFChars(jniEnv,out,outPath);
}
