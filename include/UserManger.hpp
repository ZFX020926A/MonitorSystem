#ifndef __USERMANGER_H__
#define __USERMANGER_H__

// 管理正在登陆用户的信息和状态

#include "StructType.hpp"
#include "TcpConnection.hpp"

#include <map>
using std::map;

class UserManger
{

public:
    static UserManger& getInstance();

    // 添加用户
    void addUser(int id, const User & user);
    // 删除用户
    void delUser(int id);
    // 获取用户
    User * getUser(TcpConnectionPtr conn);
    // 修改用户状态
    void modifyUserStatus(int id, UserStatus status);

private:
    UserManger();
    ~UserManger();

    // 删除构造函数和赋值运算符
    UserManger(const UserManger&) = delete;
    UserManger& operator=(const UserManger&) = delete;

private:
    map<int, User> _users; // 用户列表
};





#endif // !__USERMANGER_H__