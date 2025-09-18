#ifndef __CAMERAPTZ_HPP__
#define __CAMERAPTZ_HPP__

#include "StructType.hpp"
#include "TcpConnection.hpp"

#include <map>
using std::map;

class cameraPTZ
{
public:
    cameraPTZ(TcpConnectionPtr conn, const Packet& packet);
    ~cameraPTZ();

    void process();

private:
    void run(const string &ptzCommand);

private:
    TcpConnectionPtr _conn;
    Packet _packet;
    map<string, string> _params;
};

#endif // !__CAMERAPTZ_HPP__