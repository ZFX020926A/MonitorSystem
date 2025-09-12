#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "InetAddress.hpp"
#include "Socket.hpp"

class Acceptor
{
public:
	Acceptor(const string & ip, const string & port);
	void ready();

    int getSockfd() const;
   
	int setAccept();
private:
	void setReuseAddr(bool on);
	void setReusePort(bool on);
	void setBind();
	void setListen();
private:
	InetAddress _inetaddr;
	Socket _sockfd;
};

#endif


