#ifndef __HANDLETASKPROCESS_H__
#define __HANDLETASKPROCESS_H__

#include "StructType.hpp"
#include "TcpConnection.hpp"

// 登录第一阶段
class UserLoginSection1
{
public:
    UserLoginSection1(TcpConnectionPtr conn, const Packet & p)
    : _conn(conn)
    , _packet(p)
    {}

    //业务逻辑的处理
    void process();

private:
    void getSetting(string & s, const char * passwd);

private:
    TcpConnectionPtr _conn;
    Packet _packet;
};


// 登录第二阶段
class UserLoginSection2
{
public:
    UserLoginSection2(TcpConnectionPtr conn, const Packet & p)
    : _conn(conn)
    , _packet(p)
    {}

    //业务逻辑的处理
    void process();


private:
    TcpConnectionPtr _conn;
    Packet _packet;
};

#endif // !__HANDLETASKPROCESS_H__

