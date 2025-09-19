#include "TcpConnection.hpp"
#include "InetAddress.hpp"
#include "EventLoop.hpp"
#include "StructType.hpp"

#include <tcpHeader.h>

#include <sstream>


TcpConnection::TcpConnection(int fd, EventLoop * loop)
: _sock(fd)
, _socketIo(fd)
, _localAddr(getLocalAddr(fd))
, _peerAddr(getPeerAddr(fd))
, _isShutdwonWrite(false)
, _ploop(loop)
{
}

TcpConnection::~TcpConnection()
{
	if(!_isShutdwonWrite) {
		shutdown();
	}
}

string TcpConnection::receive()
{
	char buff[65536] = {0};
    //这里使用的是readline,所以消息的边界为'\n'
	_socketIo.readline(buff, sizeof(buff));
	return string(buff);
}
    
int TcpConnection::readPacket(Packet & packet)
{
    return _socketIo.readPacket(packet);
}
	
void TcpConnection::send(const string & msg)
{
	_socketIo.writen(msg.c_str(), msg.size());
}

void TcpConnection::sendInLoop(const string & msg)
{
    if(_ploop) {
        _ploop->runInLoop(std::bind(&TcpConnection::send, this, msg));
    }
}

void TcpConnection::sendInLoop(const TLV & tlv)
{
    int tlvlen = 8 + tlv.length;//8字节的TLV头 + 内容长度
	// TLV temp;
	// temp.length = htonl(tlv.length);
	// temp.type = htonl(tlv.type);

	int len = htonl(tlv.length);
	int ty = htonl(tlv.type);
	// memcpy(temp.data, tlv.data, tlv.length);
    string msg;

    // msg.assign((const char*)&temp, tlvlen);//再拷贝内容
	// msg.append((const char*)&temp, 8);
	// msg.append(tlv.data, tlv.length);

	msg.append(reinterpret_cast<const char*>(&ty), sizeof(ty));
	msg.append(reinterpret_cast<const char*>(&len), sizeof(len));
	msg.append(tlv.data, tlv.length);


    sendInLoop(msg);    
}

void TcpConnection::shutdown()
{
	if(!_isShutdwonWrite) {
		_isShutdwonWrite = true;
		_sock.shutdownWrite();
	}
}

void TcpConnection::handleConnectionCallback()
{
    if(_onConnection) {
        _onConnection(shared_from_this());
    }
}

void TcpConnection::handleMessageCallback()
{
    if(_onMessage) {
        _onMessage(shared_from_this());
    }
}

void TcpConnection::handleCloseCallback()
{
    if(_onClose) {
        _onClose(shared_from_this());
    }
}

string TcpConnection::toString() const
{
	std::ostringstream oss;
	oss << "tcp " << _localAddr.getIP() << ":" << _localAddr.getPort() << " --> "
		<< _peerAddr.getIP() << ":" << _peerAddr.getPort();
	return oss.str();
}


bool TcpConnection::isClosed() const
{
    char buff[20] = {0};
    return _socketIo.recvPeek(buff, sizeof(buff)) == 0;
}


InetAddress TcpConnection::getLocalAddr(int fd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr);
	if(getsockname(_sock.getSockFD(), (struct sockaddr*)&addr, &len) == -1) {
		perror("getsockname");
	}
	return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr(int fd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr);
	if(getpeername(_sock.getSockFD(), (struct sockaddr*)&addr, &len) == -1) {
		perror("getpeername");
	}
	return InetAddress(addr);
}


