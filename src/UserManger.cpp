#include "UserManger.hpp"

UserManger::UserManger()
{

}

UserManger::~UserManger()
{
}

UserManger& UserManger::getInstance()
{
    static UserManger UserInstance;
    return UserInstance;
}

// 添加用户
void UserManger::addUser(int id, const User & user)
{
    _users[id] = user;
}

// 删除用户
void UserManger::delUser(int id)
{
    _users.erase(id);
}

// 获取用户
User * UserManger::getUser(TcpConnectionPtr conn)
{
    for (auto& user : _users) {
        if (user.second._conn == conn) {
            return &user.second;
        }
    }
    return nullptr;
}

// 修改用户状态
void UserManger::modifyUserStatus(int id, UserStatus status)
{
    auto it = _users.find(id);
    if (it != _users.end()) {
        it->second._status = status;
    }
}
