#ifndef __STRUCTTYPE_H__
#define __STRUCTTYPE_H__

#include <string>
#include <cstring>

using std::string;


// 接收消息时 使用的包结构 
class Packet
{
    Packet(int type, int length, const string &msg)
    : _type(type)
    , _length(length)
    , _msg(msg)
    {}

    ~Packet()
    {}

    int getType() const 
    { 
        return _type; 
    }

    int getLength() const 
    { 
        return _length; 
    }

    string getMsg() const 
    { 
        return _msg; 
    }

private:
    int _type; // 此条消息的类型
    int _length; // 此条消息的长度
    string _msg;
};

// 发送消息时使用的 TLV
class TLV
{
    TLV(int type, int length, const char *value)
    : _type(type)
    , _length(length)
    {
        memset(&_data, 0, sizeof(_data));
        memcpy(_data, value, length);
    }

    ~TLV()
    {}

    int getType() const 
    { 
        return _type; 
    }

    int getLength() const 
    { 
        return _length; 
    }

    string getValue() const 
    { 
        return string(_data, _length); 
    }
private:
    int _type; // 此条消息的类型
    int _length; // 此条消息的长度
    char _data[1024]; // 此条消息的值
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
