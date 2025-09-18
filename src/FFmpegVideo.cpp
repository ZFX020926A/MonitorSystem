#include "FFmpegVideo.hpp"

#include "LogManger.hpp"

#include "StructType.hpp"
#include "TcpConnection.hpp" 

#include <iostream>
#include <unistd.h> // for sleep
using std::cout;
using std::endl;


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
        //sendToQtClient();

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
void FFmpegVideo::sendToQtClient()
{
    if (_conn || !_conn->isClosed()) 
    {
    // 记录日志，避免写入
        cout << "sendToQtClient: _conn is valid, sending packet." << endl;
    // TLV tlv;
    // tlv.type = TASK_TYPE_GET_VIDEO_STREAM_RESP_OK;
    // int maxDataLen = 1024;
    // int dataLen = _avPacket->size > maxDataLen ? maxDataLen : _avPacket->size;

    // // 按照VideoFrame结构体序列化，且全部转为网络字节序
    // std::vector<char> frameData;
    // int cameraId = 0; // TODO: 替换为实际cameraId
    // uint32_t netCameraId = htonl(cameraId);
    // frameData.insert(frameData.end(), (char*)&netCameraId, (char*)&netCameraId + sizeof(netCameraId));

    // uint32_t netSize = htonl(_avPacket->size);
    // frameData.insert(frameData.end(), (char*)&netSize, (char*)&netSize + sizeof(netSize));

    // uint64_t netPts = htobe64(_avPacket->pts);
    // frameData.insert(frameData.end(), (char*)&netPts, (char*)&netPts + sizeof(netPts));

    // uint64_t netDts = htobe64(_avPacket->dts);
    // frameData.insert(frameData.end(), (char*)&netDts, (char*)&netDts + sizeof(netDts));

    // frameData.insert(frameData.end(), (char*)_avPacket->data, (char*)_avPacket->data + dataLen);

    // // 打印序列化内容
    // printf("VideoFrame序列化: cameraId=%d, size=%d, pts=%ld, dts=%ld, dataLen=%d\n",
    //        cameraId, _avPacket->size, _avPacket->pts, _avPacket->dts, dataLen);
    // printf("序列化总长度: %zu\n", frameData.size());
    // printf("前16字节: ");
    // for (size_t i = 0; i < 16 && i < frameData.size(); ++i)
    // {
    //     printf("%02X ", (unsigned char)frameData[i]);
    // }

    // printf("\n");

    // // 拷贝到TLV
    // tlv.length = frameData.size();
    // memcpy(tlv.data, frameData.data(), tlv.length);
    // _conn->sendInLoop(tlv);
    // sleep(5);
    }
}


