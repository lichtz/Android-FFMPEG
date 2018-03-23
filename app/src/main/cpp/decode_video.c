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
#include "libavutil/imgutils.h"


#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO ,"zyl",__VA_ARGS__);
#define INBUF_SIZE 4096



JNIEXPORT void JNICALL Java_com_zero_licht_ffmpeg_video_1utils_like
        (JNIEnv * jniEnv, jclass jclass, jstring in, jstring out) {
    //需要回收
    const char *inPath = (*jniEnv)->GetStringUTFChars(jniEnv, in, NULL);
    const char *outPath = (*jniEnv)->GetStringUTFChars(jniEnv, out, NULL);
     //注册组建
    av_register_all();
    avcodec_register_all();
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
    const AVCodec  *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
    LOGI("zyl 看看看看看%d\n",avCodec->id);
//    AVCodecParserContext *avCodecParserContext = av_parser_init(avCodec->id);

    AVCodecContext * avCodecContext = avcodec_alloc_context3(avCodec);
    if (avcodec_open2(avCodecContext,avCodec,NULL)<0){
        return;
    }

    LOGI("zyl 看看看看看%d\n", avCodecContext->pix_fmt);
    //解码准备工作

    //求出这个编码下一帧需要多大空间
    size_t yuv_420_frame_size = (size_t) av_image_get_buffer_size(AV_PIX_FMT_YUV420P,avCodecParameters->width,
                                                       avCodecParameters->height,1);
    //申请一块内存空间
    uint8_t  *out_buffer =(uint8_t * ) av_malloc(yuv_420_frame_size);
     AVFrame * avframe_yuv420 =  av_frame_alloc();
    //初始化
    av_image_fill_arrays(avframe_yuv420->data,avframe_yuv420->linesize
    ,out_buffer,AV_PIX_FMT_YUV420P,avCodecParameters->width,
    avCodecParameters->height,1);
    struct SwsContext *swsContext = sws_getContext(avCodecParameters->width,avCodecParameters->height
            ,12,avCodecParameters->width,avCodecParameters->height,
                                                   AV_PIX_FMT_YUV420P,SWS_BICUBIC ,NULL,NULL,NULL);
    LOGI("zyl  ————————————————————————————————————————————");


    //写如文件

   FILE *file_out =  fopen(outPath,"wb");
    if (file_out==NULL){
        LOGI("输出文件打开失败");
        return;
    }

    int  current_index =0;
    //读取视频压缩数据 so AVPacket
    AVPacket * avPacket =  av_packet_alloc();
    AVFrame *  avFrame=  av_frame_alloc();
    while (av_read_frame(avFormatContext,avPacket)>=0){
        //找到视频流
        if (avPacket->stream_index ==video_index){
            //开始解码
            LOGI("zyl 开始解码");
            //发送一帧压缩数据
            int send=    avcodec_send_packet(avCodecContext,avPacket);
            LOGI("zyl 开始解码  发送dend %d\n",send);
            //解码一帧视频压缩数据
          int decode_result =   avcodec_receive_frame(avCodecContext,avFrame);
            if (decode_result == AVERROR(EAGAIN)){
                LOGI("zyl 解码EAGAIN");
            }
            if (decode_result == AVERROR_EOF){
                LOGI("zyl AVERROR_EOF");
            }
            if (decode_result == AVERROR(EINVAL)){
                LOGI("zyl AVERROR(EINVAL)");
            }
            LOGI("zyl 解码%d\n",decode_result);
                if(decode_result==0){
                //解码成功，进行转码
            int  ss=    sws_scale(swsContext,(const uint8_t *const *)avFrame->data,avFrame->linesize,0,avCodecContext->height,
                          avframe_yuv420->data,avframe_yuv420->linesize );

                LOGI("zyl %d\n",ss);



            }



        }


    }


    (*jniEnv)->ReleaseStringUTFChars(jniEnv,in,inPath);
    (*jniEnv)->ReleaseStringUTFChars(jniEnv,out,outPath);


}