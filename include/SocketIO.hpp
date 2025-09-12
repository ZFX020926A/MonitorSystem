#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

#include "StructType.hpp"

class SocketIO
{
public:
	SocketIO(int fd);

    // 读取len个字节到buff中，返回实际读取的字节数
	int readn(char * buff, int len);

    // 读取一行，返回实际读取的字节数
	int readline(char * buff, int maxlen);

    // 读取一个数据包，返回实际读取的字节数
    int readPacket(Packet & packet);

    // 发送len个字节，返回实际发送的字节数
	int writen(const char * buff, int len);

    // 查看缓冲区数据，不移除，返回实际读取的字节数
	int recvPeek(char * buff, int len) const;

private:
	int _fd;
};

#endif
