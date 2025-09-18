#ifndef __FFMPEGVIDEO_H__
#define __FFMPEGVIDEO_H__

#include "TcpConnection.hpp"
#include "StructType.hpp"

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
    FFmpegVideo(TcpConnectionPtr conn, const Packet & p);
    ~FFmpegVideo();

    void run();
    // 打开视频流
    bool openVideo();

    // 读取视频帧
    bool readFrame();
    // 发送视频帧到Qt客户端
    void sendToQtClient();
private:

    TcpConnectionPtr _conn;
    Packet _packet;

    // 接收摄像头的URL 
    string _url;
    // 封装格式上下文 存储了视频的封装信息 
    AVFormatContext* _avFmCtx;
    // 存储一帧压缩数据
    AVPacket* _avPacket;
    // 视频流索引
    int _videoStreamIndex;
};


#endif // !__FFMPEGVIDEO_HPP__