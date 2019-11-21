//
// Created by 刘帅帅 on 2019/11/20.
//
#include "video_player.h"
#define TAG "VideoPlayer"

void init() {
    av_register_all();
    //封装格式上下文
    formatContext = avformat_alloc_context();
}

int open(const char *videoPath, const char *outPath) {
    //打开输入视频文件
    avformat_open_input(&formatContext, videoPath, NULL, NULL);
    //获取视频流信息
    avformat_find_stream_info(formatContext, NULL);

    //获取视频流的索引位置AVStream[index]
    int videoStreamIdx = -1;
    int i = 0;
    for (; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIdx = i;
            break;
        }
    }

    //获取解码器
    AVCodecContext *codecContext = formatContext->streams[videoStreamIdx]->codec;
    AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);

    avcodec_open2(codecContext, codec, NULL);
    //解码前的数据(压缩数据)
    AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    //解码后的数据
    AVFrame *frame = av_frame_alloc();
    AVFrame *yuv_frame = av_frame_alloc();
    //只有制定了AVFrame的像素格式，画面大小才能真正分配内存
    //缓冲区分配内存
    uint8_t * outBuffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,codecContext->width,codecContext->height));
    //设置yuvFrame的缓冲区，像素格式
    avpicture_fill((AVPicture *)yuv_frame,outBuffer,AV_PIX_FMT_YUV420P,codecContext->width,codecContext->height);
    FILE *outFile = fopen(outPath, "wb");
    //用于像素格式转换或缩放
    struct SwsContext *swsContext = sws_getContext(codecContext->width, codecContext->height,
                                                   codecContext->pix_fmt, codecContext->width,
                                                   codecContext->height, AV_PIX_FMT_YUV420P,
                                                   SWS_BILINEAR, NULL, NULL, NULL);
    int got_frame;
    int framcount = 0;
    while (av_read_frame(formatContext, packet) >= 0) {
        if(packet->stream_index == videoStreamIdx) {
            //AVPacket转为AVFrame
            avcodec_decode_video2(codecContext, frame, &got_frame, packet);
            //解码成功
            if (got_frame > 0) {
                //转为制定的YUV420P像素
                sws_scale(swsContext, frame->data, frame->linesize, 0,
                          frame->height, yuv_frame->data, yuv_frame->linesize);

                //向yuv文件保存解码之后的帧数据
                //一个像素包含一个Y
                int y_size = codecContext->width * codecContext->height;
                fwrite(yuv_frame->data[0], 1, y_size, outFile);
                fwrite(yuv_frame->data[1], 1, y_size / 4, outFile);
                fwrite(yuv_frame->data[2], 1, y_size / 4, outFile);
                LOGI(TAG, "解码%d帧", framcount++);
            }
        }
        av_free_packet(packet);
    }
    LOGI(TAG,"解码完成");

    av_frame_free(&yuv_frame);
    avcodec_free_context(codecContext);
    avformat_free_context(formatContext);
    return 0;
}