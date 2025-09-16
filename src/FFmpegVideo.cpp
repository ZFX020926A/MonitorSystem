#include "FFmpegVideo.hpp"

#include "LogManger.hpp"

#include <iostream>
using std::cout;
using std::endl;


FFmpegVideo::FFmpegVideo(const string &url)
: _url(url)
, _avFmCtx(nullptr)
, _avCodecCtx(nullptr)
, _avCodec(nullptr)
, _avFrame(nullptr)
, _avFrameRGB(nullptr)
, _avPacket(nullptr)
, _imgConvertCtx(nullptr)
, _outBuffer(nullptr)
{
    // 初始化ffmpeg网络模块
    avformat_network_init();
    _avPacket = av_packet_alloc();
    _avFrame = av_frame_alloc();
}

FFmpegVideo::~FFmpegVideo()
{
    if(_outBuffer) av_free(_outBuffer);
    if(_avFrameRGB) av_frame_free(&_avFrameRGB);
    if(_imgConvertCtx) sws_freeContext(_imgConvertCtx);
    if(_avCodecCtx) {
        avcodec_close(_avCodecCtx);
        avcodec_free_context(&_avCodecCtx);
    }
    if(_avFmCtx) {
        avformat_close_input(&_avFmCtx);
        avformat_free_context(_avFmCtx);
    }
    if(_avPacket) av_packet_free(&_avPacket);
    if(_avFrame) av_frame_free(&_avFrame);
    avformat_network_deinit();
}


   // 打开视频流
bool FFmpegVideo::openVideo()
{
    // 初始化选项 强制使用Tcp 确保数据传输的完整性
    AVDictionary* opts = nullptr;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    // 20秒
    av_dict_set(&opts, "analyzeduration", "20000000", 0); 
    // 50MB
    av_dict_set(&opts, "probesize", "50000000", 0);  
    
    // 打开输入流
    int openret = avformat_open_input(&_avFmCtx, _url.c_str(), nullptr, &opts);
    // 返回0 正确 负数 错误
    if(openret < 0)
    {
        // 打开输入流失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : open input stream failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : open input stream failed for url: " << endl;
        return false;
    }

    // 打印输入流信息
    av_dump_format(_avFmCtx, 0, _url.c_str(), 0);

    // 打开输入流成功
    // 查找流信息
    int findret = avformat_find_stream_info(_avFmCtx, nullptr);
    if(findret < 0)
    {
        // 查找流信息失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : find stream info failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : find stream info failed for url: " << endl;
        return false;
    }

    // 查找流信息成功 暂时先不处理音频信息
    // 查找视频流索引
    int findbestret = av_find_best_stream(_avFmCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &_avCodec, 0);
    if(findbestret < 0)
    {
        // 查找视频流索引失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : find best stream failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : find best stream failed for url: " << endl;
        return false;       
    }

    // 查找视频流索引成功
    _videoStreamIndex = findbestret; // 保存视频流索引
    // 获取编解码上下文
    _avCodecCtx = avcodec_alloc_context3(_avCodec);
    int avcodecret = avcodec_parameters_to_context(_avCodecCtx, _avFmCtx->streams[findbestret]->codecpar);

    if(avcodecret < 0)
    {
        // 分配编解码上下文失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : allocate codec context failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : allocate codec context failed for url: " << endl;
        return false;       
    }

    // 分配编解码上下文成功
    // 打开编解码器
    int avcodecopenret = avcodec_open2(_avCodecCtx, _avCodec, nullptr);
    if(avcodecopenret < 0)
    {
        // 打开编解码器失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : open codec failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : open codec failed for url: " << endl;
        return false;       
    }

    // 打开编解码器成功
    // 初始化图像转换上下文
    _imgConvertCtx = sws_getContext(
        _avCodecCtx->width, _avCodecCtx->height, _avCodecCtx->pix_fmt,
        _avCodecCtx->width, _avCodecCtx->height, AV_PIX_FMT_RGB32,
        SWS_BICUBIC, nullptr, nullptr, nullptr);
    
    if(!_imgConvertCtx)
    {
        // 初始化图像转换上下文失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : initialize image convert context failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : initialize image convert context failed for url: " << endl;
        return false;
    }
    
    // 初始化图像转换上下文成功
    // 计算RGB32所需的缓冲区大小
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, _avCodecCtx->width, _avCodecCtx->height);
    // 分配缓冲区
    _outBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    if(!_outBuffer)
    {
        // 分配缓冲区失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : allocate output buffer failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : allocate output buffer failed for url: " << endl;
        return false;
    }
    // 分配缓冲区成功
    // 分配RGB帧
    _avFrameRGB = av_frame_alloc();
    if(!_avFrameRGB)
    {
        // 分配RGB帧失败    
        LogManger::getInstance().error("FFmpegVideo::openVideo() : allocate RGB frame failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : allocate RGB frame failed for url: " << endl;
        return false;
    }
    // 分配RGB帧成功
    // 将缓冲区与RGB帧关联
    avpicture_fill((AVPicture *)_avFrameRGB, _outBuffer, AV_PIX_FMT_RGB32, _avCodecCtx->width, _avCodecCtx->height);
    // 将转换后的RGB帧 发送给Qt客户端显示

    // _avPacket = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    // int y_size = _avCodecCtx->width * _avCodecCtx->height;
    // av_new_packet(_avPacket, y_size); //分配packet的数据

    // 怎么查看_avPacket中的内容？

}

    // 关闭视频流
// void FFmpegVideo::closeVideo()
// {
//     if(_avFrame) av_frame_free(&_avFrame);
//     if(_avFrameRGB) av_frame_free(&_avFrameRGB);
//     if(_outBuffer) av_free(_outBuffer);
//     if(_imgConvertCtx) sws_freeContext(_imgConvertCtx);
//     if(_avPacket) { av_packet_unref(_avPacket); free(_avPacket); }
//     if(_avCodecCtx) 
//     {
//         avcodec_close(_avCodecCtx);
//         avcodec_free_context(&_avCodecCtx);
//     }
//     if(_avFmCtx) 
//     {
//         avformat_close_input(&_avFmCtx);
//         avformat_free_context(_avFmCtx);
//     }
//     // 关闭ffmpeg网络模块
//     avformat_network_deinit();
// }

    // 读取视频帧
bool FFmpegVideo::readFrame()
{
    while (true)
    {
        av_packet_unref(_avPacket);
        // 读取视频帧
        int readVideoret = av_read_frame(_avFmCtx, _avPacket);
        if(readVideoret < 0)
        {
            // 读取视频帧失败
            LogManger::getInstance().error("FFmpegVideo::readFrame() : read video frame failed for url: " + _url);
            cout << "FFmpegVideo::readFrame() : read video frame failed for url: " << endl;
            return false;       
        }
        if(_avPacket->stream_index != _videoStreamIndex) {
            // 不是视频流
            continue;
        }
        // 读取视频帧成功
        // 解码视频帧
        int got_picture = 0;
        int avcodecdecoderet = avcodec_decode_video2(_avCodecCtx, _avFrame, &got_picture, _avPacket);
        if(avcodecdecoderet < 0)
        {
            // 解码视频帧失败
            LogManger::getInstance().error("FFmpegVideo::readFrame() : decode video frame failed for url: " + _url);
            cout << "FFmpegVideo::readFrame() : decode video frame failed for url: " << endl;
            return false;
        }

        // 打印视频帧信息
        printf("AVPacket info:\n");
        printf("  size: %d\n", _avPacket->size);
        printf("  pts: %" PRId64 "\n", _avPacket->pts);
        printf("  dts: %" PRId64 "\n", _avPacket->dts);
        printf("  stream_index: %d\n", _avPacket->stream_index);
        printf("  first 8 bytes: ");
        for (int i = 0; i < 8 && i < _avPacket->size; ++i) {
            printf("%02x ", _avPacket->data[i]);
        }
        printf("\n");

        if (_avFrame) {
            printf("AVFrame info:\n");
            printf("  width: %d\n", _avFrame->width);
            printf("  height: %d\n", _avFrame->height);
            printf("  format: %d\n", _avFrame->format);
            printf("  key_frame: %d\n", _avFrame->key_frame);
            printf("  pict_type: %d\n", _avFrame->pict_type);
        }
    
        // 解码视频帧成功
        if(got_picture)
        {
            // 转换成RGB32格式
            sws_scale(_imgConvertCtx, (const uint8_t* const*)_avFrame->data, _avFrame->linesize, 0, _avCodecCtx->height, _avFrameRGB->data, _avFrameRGB->linesize);
            // 发送给Qt客户端
            cout << "准备好发送给Qt客户端" << endl;
            // 伪代码
            // sendToQtClient(_avFrameRGB->data[0], _avCodecCtx->width, _avCodecCtx->height);
        }
    }
}

//     // 转换成RGB32格式
// bool FFmpegVideo::convertToRGB32()
// {

// }

//     // 发送给Qt客户端
// bool FFmpegVideo::sendToQtClient()
// {

// }


