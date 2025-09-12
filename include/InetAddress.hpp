#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <tcpHeader.h>
#include <string>

using std::string;

class InetAddress
{
public:
    InetAddress(const string& ip, const string &port);
    InetAddress(const struct sockaddr_in &addr);
    ~InetAddress();

    string getIP() const;
    string getPort() const;
    struct sockaddr_in *getSockAddrInet();

private:
    struct sockaddr_in _addrin;
};


#endif // !__INETADDRESS_H__