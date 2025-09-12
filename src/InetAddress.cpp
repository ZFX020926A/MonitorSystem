#include "InetAddress.hpp"

#include <iostream>

// using std::cout;
// using std::endl;

using std::to_string;

InetAddress::InetAddress(const string& ip, const string &port)
{
    //cout << "InetAddress(const string& ip, const string &port)" << ip << port << endl;
    memset(&_addrin, 0, sizeof(_addrin));
    _addrin.sin_family = AF_INET;
    _addrin.sin_addr.s_addr = inet_addr(ip.c_str());
    _addrin.sin_port = htons(atoi(port.c_str()));
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
: _addrin(addr)
{
}

InetAddress::~InetAddress()
{
}

string InetAddress::getIP() const
{
    return string(inet_ntoa(_addrin.sin_addr));
}

string InetAddress::getPort() const
{
    return to_string(ntohs(_addrin.sin_port));
}

struct sockaddr_in *InetAddress::getSockAddrInet()
{
    return &_addrin;
}
