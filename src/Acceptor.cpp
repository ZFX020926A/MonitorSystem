#include "Acceptor.hpp"

#include <tcpHeader.h>



Acceptor::Acceptor(const string &ip, const string &port)
: _inetaddr(ip, port)
, _sockfd()
{

}

void Acceptor::ready()
{
	setReuseAddr(true);
	setReusePort(true);
	setBind();
	setListen();
}

int Acceptor::getSockfd() const
    {    
        return _sockfd.getSockFD();    
    }

void Acceptor::setReuseAddr(bool on)
{
	int one = on;
	if(setsockopt(_sockfd.getSockFD(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) 
    {
		perror("setsockopt");	
	}
}

void Acceptor::setReusePort(bool on)
{
	int one = on;
	if(setsockopt(_sockfd.getSockFD(), SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one)) < 0) 
    {
		perror("setsockopt");	
	}
}

void Acceptor::setBind()
{
	int ret = bind(_sockfd.getSockFD(), (struct sockaddr *)_inetaddr.getSockAddrInet(), sizeof(struct sockaddr));
	if(-1 == ret) {
		perror("bind");
	}
}

void Acceptor::setListen()
{
	int ret = ::listen(_sockfd.getSockFD(), 20000);
	if(-1 == ret) {
		perror("listen");
	}
}

int Acceptor::setAccept()
{
	int accfd = accept(_sockfd.getSockFD(), NULL, NULL);
	if(accfd == -1) {
		perror("accept");
	}
	return accfd;
}


