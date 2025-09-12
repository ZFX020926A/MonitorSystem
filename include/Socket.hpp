#ifndef __SOCKET_H__
#define __SOCKET_H__

// 所有和文件描述符相关的操作 都放在这个类中

class Socket
{

public:
    Socket();
    Socket(int fd);
    ~Socket();

    int getSockFD() const;
    void shutdownWrite();
    

private:
    int _sockfd;
};


#endif // !__SOCKET_H__
