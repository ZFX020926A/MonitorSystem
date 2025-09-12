#ifndef __STRUCTTYPE_H__
#define __STRUCTTYPE_H__

// 此类中 将使用到的packet tlv等结构体进行封装
// 同时 还定义了 enum TaskType用来表示packet和tlv的消息类型

#include <string>
#include <cstring>

using std::string;


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
};


#endif // !__STRUCTTYPE_H__
