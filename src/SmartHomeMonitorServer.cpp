#include "SmartHomeMonitorServer.hpp"
#include "HandleTaskProcess.hpp"
#include "StructType.hpp"
#include "LogManger.hpp"
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
    case TASK_TYPE_LOGIN_SECTION2:
        {}
        break;
    }
}


void SmartHomeMonitorServer::onClose(TcpConnectionPtr conn)
{
    LogManger::getInstance().info("tcp"+ conn->toString() + "has closed.\n");
}


