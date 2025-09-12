#ifndef __SmartHomeMonitorServer_H__
#define __SmartHomeMonitorServer_H__

#include "ThreadPool.hpp"
#include "TcpServer.hpp"

class SmartHomeMonitorServer
{
public:
    SmartHomeMonitorServer(const string &ip, const string &port, int threadNum, int taskSize);

    void start();
    void stop();
private:
    void onConnection(TcpConnectionPtr conn);
    void onMessage(TcpConnectionPtr conn);
    void onClose(TcpConnectionPtr conn);

private:
    ThreadPool _threadpool;
    TcpServer _server;
};


#endif

