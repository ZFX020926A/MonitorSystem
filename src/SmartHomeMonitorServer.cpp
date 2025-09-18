#include "SmartHomeMonitorServer.hpp"
#include "HandleTaskProcess.hpp"
#include "StructType.hpp"
#include "LogManger.hpp"
#include "cameraPTZ.hpp"
#include "FFmpegVideo.hpp"
#include <iostream>

using std::cout;
using std::endl;


SmartHomeMonitorServer::SmartHomeMonitorServer(const string &ip, const string &port, int threadNum, int taskSize)
: _threadpool(threadNum, taskSize)
, _server(ip, port)
{
    using namespace std::placeholders;
    _server.setAllCallbacks(
        std::bind(&SmartHomeMonitorServer::onConnection, this, _1),
        std::bind(&SmartHomeMonitorServer::onMessage, this, _1),
        std::bind(&SmartHomeMonitorServer::onClose, this, _1));
}

void SmartHomeMonitorServer::start()
{
    _threadpool.ThreadPoolStart();
    _server.start();
}


void SmartHomeMonitorServer::stop()
{
    _server.stop();
    _threadpool.ThreadPoolStop();
}

void SmartHomeMonitorServer::onConnection(TcpConnectionPtr conn)
{
    cout << "has connected." << endl;
    LogManger::getInstance().info("tcp"+ conn->toString() + "has connected.\n");
}

void SmartHomeMonitorServer::onMessage(TcpConnectionPtr conn)
{
    cout << "onMessage..." << endl;
    Packet packet;//解析TLV格式之后的消息放在packet中
    int ret = conn->readPacket(packet);
    cout << "read:" << ret << "bytes.\n";
    cout << "packet.type: " << packet.type << endl
         << "packet.length:" << packet.length << endl
         << "pakcet.msg:" << packet.msg << endl;

    
    switch(packet.type)
    {
    case TASK_TYPE_LOGIN_SECTION1:
        {
            UserLoginSection1 userLogin1(conn, packet);
            _threadpool.addTask(std::bind(&UserLoginSection1::process, userLogin1));
        }
        break;
    case TASK_TYPE_REGISTER1:
        {
            UserLoginSection1 userLogin1(conn, packet);
            _threadpool.addTask(std::bind(&UserLoginSection1::process, userLogin1));
        }
        break;
    case TASK_TYPE_LOGIN_SECTION2:
        {
            UserLoginSection2 userLogin2(conn, packet);
            _threadpool.addTask(std::bind(&UserLoginSection2::process, userLogin2));
        }
        break;
    case TASK_TYPE_REGISTER2:
        {
            UserLoginSection2 userLogin2(conn, packet);
            _threadpool.addTask(std::bind(&UserLoginSection2::process, userLogin2));
        }
        break;
        case TASK_TYPE_CAMERA_PTZ_CONTROL: // 处理摄像头控制
        {
            cameraPTZ cameraPTZ(conn, packet);
            _threadpool.addTask(std::bind(&cameraPTZ::process, cameraPTZ));
        }
        break;
    case TASK_TYPE_GET_VIDEO_STREAM: // 处理获取视频流
        {
            auto ffmpegvideo = std::make_shared<FFmpegVideo>(conn, packet);
            _threadpool.addTask([ffmpegvideo]() { 
                ffmpegvideo->run(); 
            });
        }
        break;
    }
}


void SmartHomeMonitorServer::onClose(TcpConnectionPtr conn)
{
    LogManger::getInstance().info("tcp"+ conn->toString() + "has closed.\n");
}


