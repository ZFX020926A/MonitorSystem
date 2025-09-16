#ifndef __FFMPEGVIDEO_H__
#define __FFMPEGVIDEO_H__

#include <string>
using std::string;

// 处理视频流
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

class FFmpegVideo
{
public:
    FFmpegVideo(const string &url);
    ~FFmpegVideo();

    // 打开视频流
    bool openVideo();
    // 关闭视频流
    // void closeVideo();
    // 读取视频帧
    bool readFrame();
    // // 转换成RGB32格式
    // bool convertToRGB32();
    // // 发送给Qt客户端
    // bool sendToQtClient();

private:
    // 接收摄像头的URL 
    string _url;
    // 封装格式上下文 存储了视频的封装信息 
    AVFormatContext* _avFmCtx;
    // 编解码上下文
    AVCodecContext* _avCodecCtx;
    // 编解码器结构体
    AVCodec* _avCodec;
    // 存储一帧解码后的数据
    AVFrame* _avFrame;
    // 存储一帧RGB帧数据
    AVFrame* _avFrameRGB;
    // 存储一帧压缩数据
    AVPacket* _avPacket;
    // 图像转换上下文
    struct SwsContext* _imgConvertCtx;
    // 输出缓冲区
    uint8_t* _outBuffer;
    // 视频流索引
    int _videoStreamIndex;
};






#endif // !__FFMPEGVIDEO_HPP__