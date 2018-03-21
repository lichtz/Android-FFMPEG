//
// Created by licht on 2018/3/21.
//

#include "com_zero_licht_ffmpeg_video_utils.h"
#include <android/log.h>


#include "libavformat/avformat.h"
//解码
#include "libavcodec/avcodec.h"


#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO ,"zyl",__VA_ARGS__);
#define INBUF_SIZE 4096
//static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
//                     char *filename)
//{
//    FILE *f;
//    int i;
//
//    f = fopen(filename,"w");
//    for (i = 0; i < ysize; i++)
//        fwrite(buf + i * wrap, 1, xsize, f);
//    fclose(f);
//}
//
//
//static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
//                   const char *filename)
//{
//
//    char buff[1024];
//    int ret;
//    //将pkt报放入到AVCodecContext
//   ret =  avcodec_send_packet(dec_ctx,pkt);
//
//    if (ret<0){
//       LOGI("sending a packet for decoding");
//        return;
//    }
//    while (ret>=0){
//        //从AVCodecContext 取出frame
//         ret = avcodec_receive_frame(dec_ctx,frame);
//        if (ret==AVERROR(EAGAIN) || ret== AVERROR_EOF){
//            return;
//        } else if (ret <0){
//            LOGI("解码错误");
//             //退出程序 exit(1)
//            exit(1);
//        }
//
//        LOGI(dec_ctx->frame_number);
//        /* the picture is allocated by the decoder. no need to
//                  free it */
//        pgm_save(frame->data[0],frame->linesize[0],frame->width,frame->height,filename);
//
//    }
//
//}



static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                   const char *filename)
{
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }

        printf("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);

        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);
    }
}






JNIEXPORT void JNICALL Java_com_zero_licht_ffmpeg_video_1utils_like
        (JNIEnv * jniEnv, jclass jclass, jstring in, jstring out) {
    const char *inPath = (*jniEnv)->GetStringUTFChars(jniEnv, in, NULL);
    const char *outPath = (*jniEnv)->GetStringUTFChars(jniEnv, out, NULL);
    const AVCodec *codec;
    AVCodecParserContext *parser;
    avcodec_register_all();
    AVCodecContext *c = NULL;
    FILE *in_f;
    AVFrame *avFrame;
    int ret;
    AVPacket *pkt;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t data;
    size_t data_size;
    //将增加的后32位置为0，防止读取mpeg超出，如果32位中的前23位不为0
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    pkt = av_packet_alloc();
    if (!pkt) {
        return;
    }
    //获取解码器 decode
    AVFormatContext *avFormatContext = avformat_alloc_context();
    int a = avformat_open_input(avFormatContext, inPath, NULL, NULL);
    if (a != 0) {
        LOGI("打开输入流回去Format");
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGI("获取视频信息失败");
        return;
    }
    int i = 0;
    int video_stream_index = -1;
    for (; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }
    }
    AVCodecParameters *codecpar = avFormatContext->streams[video_stream_index]->codecpar;

    codec = avcodec_find_decoder(codecpar->codec_id);
    if (codec == NULL) {
        LOGI("无法解码");
        return;
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        LOGI("解析初始化出错");
        return;
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGI("申请解码空间");
        return;
    }

    //打开解码器
    if (avcodec_open2(c, codec, NULL) < 0) {
        LOGI("打开解码器失败");
        return;

    }
    in_f = fopen(inPath, "rb");
    if (!in_f) {
        LOGI("源文件fopen出错");
        return;
    }

    avFrame = av_frame_alloc();
    if (!avFrame) {
        LOGI("不能申请video frame 空间");
        return;
    }
    //解码开始
    while (!feof(in_f)) {
        data_size = fread(inbuf, 1, INBUF_SIZE, in_f);
        if (!data_size) {
            break;
        }
        data = inbuf;
        while (data_size > 0) {
            int ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, data_size,
                                       AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

            if (ret < 0) {
                LOGI("解码出错");
                return;
            }

            data += ret;
            data_size -= ret;

            if (pkt->size) {

            }
        }

    }


}