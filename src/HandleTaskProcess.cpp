#include "HandleTaskProcess.hpp"
#include "LogManger.hpp"

#include <iostream>

#include <shadow.h>

using std::cout;
using std::endl;

// // 登录第一阶段 验证用户名字
// void UserLoginSection1::process()
// {
//     cout << "UserLoginSection1::process()" << endl;

//     if( _packet.type == TASK_TYPE_LOGIN_SECTION1) {
//         // 用户登录
//         string username;
//         username = _packet.msg;
//         LogManger::getInstance().info("UserLoginSection1::process() : receive login username : " + username);
        
//     }

    

// }

//该函数运行在计算线程
void UserLoginSection1::process() {
    cout << "login section 1 is processing" << endl;
    //执行用户登录的阶段1操作
    if(_packet.type == TASK_TYPE_LOGIN_SECTION1) {
        //消息内容为用户名
        string username = _packet.msg;

        //从/etc/shadow文件中获取密文
        struct spwd * sp;
        sp = getspnam(username.c_str());
        if(sp == nullptr) {
            //用户名出错
            LogManger::getInstance().error("UserLoginSection1::process(): getspnam error");
            //告知对端有问题
            TLV tlv;
            tlv.type = TASK_TYPE_LOGIN_SECTION1_RESP_ERROR;
            tlv.length = 0;//不需要发送消息体了
            _conn->sendInLoop(tlv);
            return;
        }

        //获取要发送给对端的setting（盐值）
        string setting;
        getSetting(setting, sp->sp_pwdp);
        LogManger::getInstance().info("UserLoginSection1::process(): get setting : " + setting);
        //构造TLV，发送给对端（成功）
        TLV tlv;
        tlv.type = TASK_TYPE_LOGIN_SECTION1_RESP_OK;
        tlv.length = setting.length();
        strncpy(tlv.data, setting.c_str(), tlv.length);
        _conn->sendInLoop(tlv);
        //cout << "send tlv to client : " << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
    }
} 

void UserLoginSection1::getSetting(string & s, const char * passwd)
{
    int i,j;
    //取出salt,i 记录密码字符下标，j记录$出现次数
    for(i = 0,j = 0;passwd[i]&& j!=4;++i)
    {
        if(passwd[i] == '$')
            ++j;
    }
    char buff[128] = {0};
    strncpy(buff, passwd, i);
    //拷贝到s中
    s.assign(buff, i);
}