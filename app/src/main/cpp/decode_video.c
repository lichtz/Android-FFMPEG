//
// Created by licht on 2018/3/21.
//

#include "libavformat/avformat.h"
//解码
#include "libavcodec/avcodec.h"
#include "com_zero_licht_ffmpeg_video_utils.h"
#include <android/log.h>
#include "libswscale/swscale.h"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO ,"zyl",__VA_ARGS__);
#define INBUF_SIZE 4096
JNIEXPORT void JNICALL Java_com_zero_licht_ffmpeg_video_1utils_like
        (JNIEnv * jniEnv, jclass jclass, jstring in, jstring out) {
    const char *inPath = (*jniEnv)->GetStringUTFChars(jniEnv, in, NULL);
    const char *outPath = (*jniEnv)->GetStringUTFChars(jniEnv, out, NULL);
    const AVCodec *codec;
    AVCodecParserContext * parser;
    avcodec_register_all();
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
   //将增加的后32位置为0，防止读取mpeg超出，如果32位中的前23位不为0
    memset(inbuf+INBUF_SIZE,0,AV_INPUT_BUFFER_PADDING_SIZE);
    //获取解码器 decode
    AVFormatContext *avFormatContext = avformat_alloc_context();
    int a = avformat_open_input(avFormatContext,inPath,NULL,NULL);
    if (a !=0){
        LOGI("打开流失败");
        return;
    }
    if (avformat_find_stream_info(avFormatContext,NULL)<0){
        LOGI("获取视频信息失败");
        return;
    }
    int i =0;
    int video_stream_index=-1;
    for (; i <avFormatContext->nb_streams ; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
        }
    }
    AVCodecParameters *codecpar = avFormatContext->streams[video_stream_index]->codecpar;

    codec= avcodec_find_decoder(codecpar->codec_id);
    if (codec == NULL){
        LOGI("无法解码");
        return;
    }

    parser = av_parser_init(codec->id);
    if (!parser){
        LOGI("解析初始化出错");
        return;
    }
    AVCodecContext *avCodecContext =avcodec_alloc_context3(codec);
    if (!avCodecContext){
        LOGI("申请解码空间");
        return;
    }

}