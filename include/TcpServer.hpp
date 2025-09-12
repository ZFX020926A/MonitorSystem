#ifndef __TcpServer_H__
#define __TcpServer_H__
#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"


class TcpServer
{
public:
    TcpServer(const string ip, const string &port)
    : _acceptor(ip, port)
    , _loop(_acceptor)
    {}

    void start() {
        _acceptor.ready();
        _loop.loop();
    }

    void stop() {
        _loop.unloop();
    }

    void setAllCallbacks(TcpConnectionCallback && cb1,
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {
        _loop.setAllCallbacks(std::move(cb1),
                              std::move(cb2),
                              std::move(cb3));
    }

private:
    Acceptor _acceptor;
    EventLoop _loop;
};


#endif

