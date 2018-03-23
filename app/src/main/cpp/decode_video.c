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



static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                   const char *filename){
    char buf[1024];
    int ret;
   ret =  avcodec_send_packet(dec_ctx,pkt);
    LOGI("zyl ret===%d\n",ret);
    if (ret >=0){
      ret =   avcodec_receive_frame(dec_ctx,frame);
        LOGI("zyl 接收===%d\n",ret);
    }

}


JNIEXPORT void JNICALL Java_com_zero_licht_ffmpeg_video_1utils_like
        (JNIEnv * jniEnv, jclass jclass, jstring in, jstring out) {
    //需要回收
    const char *inPath = (*jniEnv)->GetStringUTFChars(jniEnv, in, NULL);
    const char *outPath = (*jniEnv)->GetStringUTFChars(jniEnv, out, NULL);
    uint8_t  inbuf[INBUF_SIZE+AV_INPUT_BUFFER_PADDING_SIZE];
    memset(inbuf+INBUF_SIZE,0,AV_INPUT_BUFFER_PADDING_SIZE);
    size_t   data_size;
    uint8_t *data;
    AVPacket *pkt;
     //注册组建
    int ret;
//    av_register_all();
    avcodec_register_all();
    //打开封装  ，取得封装的上下文
     //保存整个视频的编码器解码器 信息 码率帧率。。
//    AVFormatContext  *avFormatContext =  avformat_alloc_context();
//    int avformat_open_inout_result =  avformat_open_input(&avFormatContext,inPath,NULL,NULL);
//    if (avformat_open_inout_result !=0){
//        LOGI("zyl 打开文件失败");
//        return;
//    }
//    //查找视频流
//   int find_stream_result =   avformat_find_stream_info(avFormatContext,NULL);
//    if (find_stream_result<0){
//        LOGI("zyl","查找视频流失败");
//    }
//
//
//    //拿到视频解码器
//    int  video_index = -1;
//    int i =0;
//    for(;i<avFormatContext->nb_streams;i++){
//        if ((avFormatContext->streams[i]->codecpar->codec_type)==AVMEDIA_TYPE_VIDEO){
//             video_index = i;
//            break;
//        }
//    }
//    //根据视频流拿到解码器
//    AVStream *video_stream =  avFormatContext->streams[video_index];
//
//    AVCodecParameters * avCodecParameters =  video_stream->codecpar;
    AVCodec  *avCodec  =avcodec_find_decoder(AV_CODEC_ID_H264);


//
//       AVCodec  *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
//    LOGI("zyl 看看看看看%d\n",avCodec->id);
//    AVCodecParserContext *avCodecParserContext = av_parser_init(avCodec->id);
    AVCodecParserContext *avCodecParserContext = av_parser_init(avCodec->id);
    AVCodecContext * avCodecContext = avcodec_alloc_context3(avCodec);

   int  rrr =  avcodec_open2(avCodecContext,avCodec,NULL);
    if (rrr<0){
        return;
    }


    //写如文件

   FILE *file_in =  fopen(inPath,"rb");
    LOGI("zyl %s\n",inPath);
    if (!file_in){
        LOGI("输出文件打开失败");
        return;
    }

    AVFrame *  avFrame=  av_frame_alloc();
    if (!avFrame){
        LOGI("申请frame空间出错");
        return;
    }
    pkt = av_packet_alloc();
    if (!pkt){
        LOGI("申请Packet空间出错");
        return;
    }

    while(!feof(file_in)){
        //读取
         data_size= fread(inbuf,1,INBUF_SIZE,file_in);
        if (!data_size){
            break;
        }

        data = inbuf;
        while (data_size>0){

             ret = av_parser_parse2(avCodecParserContext,avCodecContext,&pkt->data,&pkt->size
             ,data,data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

            if (ret<0){
                LOGI("parse2出错");
                return;
            }
            LOGI("zyl bububu%d\n",ret);
            data +=ret;
            data_size -=ret;
            if (pkt->size){

               int  ret =  avcodec_send_packet(avCodecContext,pkt);
                LOGI("zyl ret===%d\n",ret);
//                decode(avCodecContext, avFrame, pkt, outPath);

            }
        }

    }


    (*jniEnv)->ReleaseStringUTFChars(jniEnv,in,inPath);
    (*jniEnv)->ReleaseStringUTFChars(jniEnv,out,outPath);


}