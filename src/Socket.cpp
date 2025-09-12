#include "Socket.hpp"

#include <tcpHeader.h>

Socket::Socket()
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_sockfd == -1)
    {
        perror("socket");
    }
}

Socket::~Socket()
{
    close(_sockfd);
}

Socket::Socket(int fd)
: _sockfd(fd)
{

}

int Socket::getSockFD() const
{
    return _sockfd;
}

void Socket::shutdownWrite()
{
    shutdown(_sockfd, SHUT_WR);
}