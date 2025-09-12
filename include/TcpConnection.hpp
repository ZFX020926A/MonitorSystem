#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__


#include "Socket.hpp"
#include "InetAddress.hpp"
#include "SocketIO.hpp"

#include <memory>
#include <string>
#include <functional>
using std::string;
using std::shared_ptr;


class EventLoop;

class TcpConnection;
using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(TcpConnectionPtr)>;

class TcpConnection
: public std::enable_shared_from_this<TcpConnection> //继承子该辅助类之后，就拥有了shared_from_this函数
{
public:
	TcpConnection(int fd, EventLoop * loop);
	~TcpConnection();

	string receive();
    int readPacket(Packet & packet);
	void send(const string & msg);
    void sendInLoop(const string & msg);
    void sendInLoop(const TLV & data);

	string toString() const;//获取五元组信息
	void shutdown();

    void setAllCallbacks(const TcpConnectionCallback & cb1,
                         const TcpConnectionCallback & cb2,
                         const TcpConnectionCallback & cb3)
    {
        _onConnection = cb1;
        _onMessage = cb2;
        _onClose = cb3;
    }

    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();
    bool isClosed() const;

private:
	InetAddress getLocalAddr(int fd);
	InetAddress getPeerAddr(int fd);
private:
	Socket _sock;
	SocketIO _socketIo;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	bool _isShutdwonWrite;//是否要主动关闭连接
    EventLoop * _ploop;

    // 回调函数
    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;

};

#endif
