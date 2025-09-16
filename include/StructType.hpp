#ifndef __STRUCTTYPE_H__
#define __STRUCTTYPE_H__

// 此类中 将使用到的packet tlv等结构体进行封装
// 同时 还定义了 enum TaskType用来表示packet和tlv的消息类型

// #include "TcpConnection.hpp" // 不使用头文件声明 TcpConnectionPtr 避免循环引用

#include <string>
#include <cstring>
#include <vector>
#include <memory> 

// 前向声明
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>; 

using std::string;
using std::vector;


// 接收消息时 使用的包结构 
struct Packet
{
    int type;
    int length;
    string msg;
};

// 发送消息时使用的 TLV
struct TLV
{
    int type;
    int length;
    char data[1024];
};

// 消息类型
enum TaskType
{
    TASK_TYPE_LOGIN_SECTION1 = 1,
    TASK_TYPE_LOGIN_SECTION1_RESP_OK,
    TASK_TYPE_LOGIN_SECTION1_RESP_ERROR,
    TASK_TYPE_LOGIN_SECTION2,
    TASK_TYPE_LOGIN_SECTION2_RESP_OK,
    TASK_TYPE_LOGIN_SECTION2_RESP_ERROR,
    TASK_TYPE_REGISTER1, // 注册第一阶段  7
    TASK_TYPE_REGISTER1_RESP_OK,
    TASK_TYPE_REGISTER1_RESP_ERROR,
    TASK_TYPE_REGISTER2,     // 注册第二阶段
    TASK_TYPE_REGISTER2_RESP_OK
};
 
enum UserStatus
{
    USER_NOT_LOGIN, // 未登录
    USER_IS_LOGINING, // 正在登录
    USER_HAS_LOGIN, // 已登录
    USER_HAS_LOGOFF // 已注销
};

struct Camera
{
    int _id; // 摄像头ID
    string _rtsp; 
    string _rtmp; 
};

struct User
{
    string _name;
    string _setting; // 盐值
    string _encrypt; // 密文
    UserStatus _status;
    TcpConnectionPtr _conn; // 该用户对应的连接
    vector<Camera> _cameras;
};


#endif // !__STRUCTTYPE_H__
