#include "FFmpegVideo.hpp"

#include "LogManger.hpp"

#include "StructType.hpp"
#include "TcpConnection.hpp" 

#include <iostream>
#include <unistd.h> 
#include <cstring>

#include <iomanip>

using std::cout;
using std::endl;
using std::hex;


FFmpegVideo::FFmpegVideo(TcpConnectionPtr conn, const Packet & p)
: _url("rtmp://192.168.105.222:1935/hlsram/live0")
, _avFmCtx(nullptr)
, _avPacket(nullptr)
, _conn(conn)
, _packet(p)
{
    // 初始化ffmpeg网络模块
    avformat_network_init();
    _avPacket = av_packet_alloc();
}

FFmpegVideo::~FFmpegVideo()
{
    if(_avFmCtx) {
        avformat_close_input(&_avFmCtx);
        avformat_free_context(_avFmCtx);
    }
    if(_avPacket) av_packet_free(&_avPacket);
    avformat_network_deinit();
}

   // 打开视频流
bool FFmpegVideo::openVideo()
{  
    // 打开输入流
    int openret = avformat_open_input(&_avFmCtx, _url.c_str(), nullptr, nullptr);
    
    // 返回0 正确 负数 错误
    if(openret < 0)
    {
        // 打开输入流失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : open input stream failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : open input stream failed for url: " << endl;
        
        // 发送失败的Tlv
        TLV tlv;
        tlv.type = TASK_TYPE_GET_VIDEO_STREAM_RESP_ERROR;
        tlv.length = 0; 
        _conn->sendInLoop(tlv);
        
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
        
        // 发送失败的Tlv
        TLV tlv;
        tlv.type = TASK_TYPE_GET_VIDEO_STREAM_RESP_ERROR;
        tlv.length = 0; 
        _conn->sendInLoop(tlv);

        return false;
    }
    
    // 查找流信息成功 暂时先不处理音频信息
    // 查找视频流索引
    int findbestret = av_find_best_stream(_avFmCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if(findbestret < 0)
    {
        // 查找视频流索引失败
        LogManger::getInstance().error("FFmpegVideo::openVideo() : find best stream failed for url: " + _url);
        cout << "FFmpegVideo::openVideo() : find best stream failed for url: " << endl;
        
        // 发送失败的Tlv
        TLV tlv;
        tlv.type = TASK_TYPE_GET_VIDEO_STREAM_RESP_ERROR;
        tlv.length = 0; 
        _conn->sendInLoop(tlv); 
        
        return false;       
    }

    // 查找视频流索引成功
    _videoStreamIndex = findbestret; // 保存视频流索引
    LogManger::getInstance().info("FFmpegVideo::openVideo() : open video stream success for url: " + _url);
    cout << "FFmpegVideo::openVideo() : open video stream success for url: " << endl;
    return true;
}

    // 读取视频帧
bool FFmpegVideo::readFrame()
{
    // cout << "readFrame: _conn=" << (_conn ? "not null" : "null") 
    // << ", isClosed=" << (_conn ? (_conn->isClosed() ? "true" : "false") : "N/A") << endl;
    
    // 这个地方有_conn的声明周期问题
    while(_conn || !_conn->isClosed())
    // while (true)
    {
        av_packet_unref(_avPacket);
        // 读取视频帧
        int readVideoret = av_read_frame(_avFmCtx, _avPacket);
        
        if(readVideoret < 0)
        {
            LogManger::getInstance().error("FFmpegVideo::readFrame() : read video frame failed for url: " + _url);
            cout << "FFmpegVideo::readFrame() : read video frame failed for url: " << endl;
            
            // 发送失败的Tlv
            TLV tlv;
            tlv.type = TASK_TYPE_GET_VIDEO_STREAM_RESP_ERROR;
            tlv.length = 0; 
            _conn->sendInLoop(tlv);

            return false;
        }
        
        if(_avPacket->stream_index != _videoStreamIndex) {
            continue;
        }
        
        // 直接转发avPacket给Qt客户端
        sendToQtClient(*_avPacket);

        // 打印详细信息
        

        printf("转发AVPacket给Qt客户端, size: %d\n", _avPacket->size);
    }
    
    return true;
}

void FFmpegVideo::run() {
    
    if (openVideo()) {
        
        readFrame();
    } else {
        std::cout << "openVideo failed, not running readFrame." << std::endl;
    }
}

    // 发送视频帧到Qt客户端
void FFmpegVideo::sendToQtClient(AVPacket &_avPacket)
{
    if (_conn || !_conn->isClosed()) 
    {
    // 记录日志，避免写入
        cout << "sendToQtClient: _conn is valid, sending packet." << endl;
        LogManger::getInstance().info("FFmpegVideo::sendToQtClient() : sending video frame to Qt client ");
        // 序列化数据 avpacket数据中数据拿取到
        VideoFrame vf;
        // vf.cameraId =
        vf.size = _avPacket.size;
        vf.pts = _avPacket.pts;
        vf.dts = _avPacket.dts;
        vf.data = _avPacket.data;
        cout << "sendToQtClient: vf.size = " << vf.size << ", pts = " << vf.pts << ", dts = " << vf.dts << endl;
        // 序列化开始
        vector<char> frameData;
        // 摄像头 头部id
        //frameData.insert(frameData.end(), reinterpret_cast<char*>(&vf.cameraId), reinterpret_cast<char*>(&vf.cameraId) + sizeof(vf.cameraId));
        uint32_t netSize = vf.size;
        frameData.insert(frameData.end(), reinterpret_cast<char*>(&netSize), reinterpret_cast<char*>(&netSize) + sizeof(netSize));
        int64_t netPts = vf.pts;
        frameData.insert(frameData.end(), reinterpret_cast<char*>(&netPts), reinterpret_cast<char*>(&netPts) + sizeof(netPts));
        int64_t netDts = vf.dts;
        frameData.insert(frameData.end(), reinterpret_cast<char*>(&netDts), reinterpret_cast<char*>(&netDts) + sizeof(netDts));
        // 视频数据
        frameData.insert(frameData.end(), reinterpret_cast<char*>(vf.data), reinterpret_cast<char*>(vf.data) + vf.size);

        // 序列化完成
        TLV tlv;
        tlv.type = TASK_TYPE_GET_VIDEO_STREAM_RESP_OK;
        tlv.length = frameData.size();
        cout << "============序列化之后的长度=========" << tlv.length << endl;
        // 直接发送 char data[65536]
        //tlv.data = {0}; // 清空数据
        memcpy(tlv.data, frameData.data(), frameData.size());
        
        // 把tlv数据详细打印出来
        // cout << "TLV Data: ";
        // for (size_t i = 0; i < tlv.length; ++i) {
        //     cout << hex << static_cast<int>(tlv.data[i]) << " ";
        // }
        // cout << endl;

        //_conn->sendInLoop(tlv);
        cout << "sendToQtClient: sent packet of type and size " << tlv.type << " " << tlv.length << endl;

        sleep(5);
    }
}


