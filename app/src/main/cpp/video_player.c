//
// Created by 刘帅帅 on 2019/11/20.
//
#include "video_player.h"
void init(){
    av_register_all();
    //封装格式上下文
    formatContext = avformat_alloc_context();
}

int open(const char * videoPath){
    //打开输入视频文件
    avformat_open_input(&formatContext,videoPath,NULL,NULL);
    //获取视频流信息
    avformat_find_stream_info(formatContext,NULL);

    //获取视频流的索引位置AVStream[index]
    int videoStreamIdx = -1;
    int i = 0;
    for(;i < formatContext->nb_streams;i++){
        if(formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStreamIdx = i;
            break;
        }
    }

    //获取解码器
    AVCodecContext* codecContext = formatContext->streams[videoStreamIdx]->codec;
    AVCodec * codec = avcodec_find_decoder(codecContext->codec_id);

    avcodec_open2(codecContext,codec,NULL);
    //解码前的数据(压缩数据)
    AVPacket* packet = av_packet_alloc();
    //解码后的数据
    AVFrame* yuv_frame = av_frame_alloc();
    int got_frame;
    while(av_read_frame(formatContext,packet) >= 0){
        //AVPacket转为AVFrame
        avcodec_decode_video2(codecContext,yuv_frame,&got_frame,packet);
        //解码成功
        if(got_frame > 0){

        }
        av_free_packet(packet);
    }

    av_frame_free(&yuv_frame);
    avcodec_free_context(codecContext);
    avformat_free_context(formatContext);
    return 0;
}