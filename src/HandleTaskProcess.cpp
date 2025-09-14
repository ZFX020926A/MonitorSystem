#include "HandleTaskProcess.hpp"
#include "LogManger.hpp"
#include "MySql.hpp"
#include "UserManger.hpp"

#include <iostream>

#include <shadow.h>

using std::cout;
using std::endl;


// 第一阶段 得到名字
void UserLoginSection1::process()
{
    cout << "UserLoginSection1::process()" << endl;

    if( _packet.type == TASK_TYPE_LOGIN_SECTION1 || _packet.type == TASK_TYPE_REGISTER1) 
    {
        // 解包获取用户名
        string username;
        username = _packet.msg;
        LogManger::getInstance().info("UserLoginSection1::process() : receive username : " + username);
        // 然后跟数据库进行交互 查询数据库中是否有该用户

        MySql myclient;
        bool ret = myclient.connect("8.148.74.118", "root", "1234", "UserMesage", 3306);
        if(!ret) {
            // 连接数据库失败
            // 告知对端有问题
            TLV tlv;
            tlv.type = TASK_TYPE_LOGIN_SECTION1_RESP_ERROR;
            tlv.length = 0; // 不需要发送消息体了
            _conn->sendInLoop(tlv);
            cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
            cout << "UserLoginSection1::process() : connect to database error" << endl;
            return ;
        }
        
        // 连接数据库成功
        string sql = "SELECT * FROM user WHERE name = '" + username + "'";
        vector<vector<string>> res = myclient.readOperationQuery(sql);

        if(res.size() == 0)
        {
            if(_packet.type == TASK_TYPE_LOGIN_SECTION1)
            {
                // 用户登录
                // 里面没有用户
                LogManger::getInstance().error("UserLoginSection1::process() : login failed, not in user : " + username);
                // 告知对端有问题
                TLV tlv;
                tlv.type = TASK_TYPE_LOGIN_SECTION1_RESP_ERROR;
                tlv.length = 0; // 不需要发送消息体了
                _conn->sendInLoop(tlv);
                cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
                return ;
            }
            else if(_packet.type == TASK_TYPE_REGISTER1)
            {
                // 没有该用户
                // 构造salt发送给对端
                string salt = GenRandomString(8);
                string setting = "$1$" + salt + "$"; // 使用MD5加密
                LogManger::getInstance().info("UserLoginSection1::process() : " + username + " register successful and generate setting : " + setting);

                // 插入 用户名 和 盐值
                string sql2 = "INSERT INTO user (name, setting) VALUES ('" + username + "', '" + setting + "')";
                myclient.writeOperationQuery(sql2);

                // 插入用户名和盐值到UserManger中
                User user;
                user._name = username;
                user._setting = setting;
                user._status = USER_NOT_LOGIN;
                user._conn = _conn;

                // 查询刚插入的用户ID
                string sql3 = "SELECT id FROM user WHERE name = '" + username + "'";
                vector<vector<string>> res2 = myclient.readOperationQuery(sql3);
                if(res2.size() != 0)
                {
                    int id = std::stoi(res2[1][0]);
                    UserManger::getInstance().addUser(id, user);
                    LogManger::getInstance().info("UserLoginSection1::process() : add new user to UserManger with id : " + std::to_string(id));
                }

                // 构造TLV 发送给对端（成功）
                TLV tlv;
                tlv.type = TASK_TYPE_REGISTER1_RESP_OK;
                tlv.length = setting.length();
                strncpy(tlv.data, setting.c_str(), tlv.length);
                _conn->sendInLoop(tlv);
            }
        }

        if(res.size() != 0)
        {
            if(_packet.type == TASK_TYPE_LOGIN_SECTION1)
            {
                // 有该用户
                // 获取该用户的setting字段
                string setting = res[1][2]; // 第一行是列名 第二行是数据 第三列是setting字段
                LogManger::getInstance().info("UserLoginSection1::process() : get " + username + " setting : " + setting);
                
                // 将用户信息添加到UserManger中
                User user;
                user._name = username;  
                user._setting = setting;
                user._status = USER_HAS_LOGIN;
                user._conn = _conn;

                user._encrypt = res[1][3];

                // 获取密文 
                // string hash;
                // getSetting(hash, res[1][3].c_str());

                cout << " <<<<< res[1][3] " << res[1][3] << endl;
                cout << " <<<<< res[1][3].c_str() " << res[1][3].c_str() << endl;
                cout << " <<<<< user._encrypt = " << user._encrypt << endl;

                int id = std::stoi(res[1][0]);
                UserManger::getInstance().addUser(id, user);
                LogManger::getInstance().info("UserLoginSection1::process() : add already existing user to UserManger with id : " + std::to_string(id));
                
                // 构造TLV 发送给对端（成功）
                TLV tlv;
                tlv.type = TASK_TYPE_LOGIN_SECTION1_RESP_OK;
                tlv.length = setting.length();
                strncpy(tlv.data, setting.c_str(), tlv.length);
                _conn->sendInLoop(tlv);

                cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
                cout << "UserLoginSection1::process() : send setting to client" << endl;
                return ;
            }
            else if(_packet.type == TASK_TYPE_REGISTER1)
            {
                // 里面有该用户
                LogManger::getInstance().error("UserLoginSection1::process() : " + username + " user already exists  ");
                // 告知对端有问题
                TLV tlv;
                tlv.type = TASK_TYPE_REGISTER1_RESP_ERROR;
                tlv.length = 0; // 不需要发送消息体了
                _conn->sendInLoop(tlv);
                cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
                return ;
            }
        }
    }
}

// 第二阶段 得到密文
void UserLoginSection2::process()
{
    // 拿取密文
    string passwd = _packet.msg;
    LogManger::getInstance().info("UserLoginSection2::process() : receive passwd : " + passwd);

    // 解包获取密文
    // string _password;
    
    // getSetting(_password, passwd.c_str());

    // 连接数据库
    MySql myclient;
    bool ret = myclient.connect("8.148.74.118", "root", "1234", "UserMesage", 3306);

    if(!ret) {
        
        // 连接数据库失败
        // 告知对端有问题
        TLV tlv;
        tlv.type = TASK_TYPE_LOGIN_SECTION2_RESP_ERROR;
        tlv.length = 0; // 不需要发送消息体了
        _conn->sendInLoop(tlv);
        cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
        cout << "UserLoginSection2::process() : connect to database error" << endl;
        return ;
    }

    if( _packet.type == TASK_TYPE_LOGIN_SECTION2)
    {
        string _userMangerPassWord;
        // 根据连接指针TcpConnectionPtr 来确定是哪一个对象 再对比其中存储的密码
        // 使用conn确定是哪一个用户的连接
        User* user = UserManger::getInstance().getUser(_conn);
        if (user) 
        {
            _userMangerPassWord = user->_encrypt;
        } 
       
        // 对比解包密文 和 根据TcpConnectionPtr指针获取的密文
        cout << "<< << UserManger" <<endl;
        cout << "<< << user->_name:" << user->_name << endl;
        cout << "<< << user->_encrypt:" << user->_encrypt << endl;

        cout << "<< << _password:" << passwd << endl;
        cout << "<< << _userMangerPassWord:" << _userMangerPassWord << endl;
        
        
        
        
        if(passwd != _userMangerPassWord)
        {
            // 密码不正确
            LogManger::getInstance().error("UserLoginSection2::process() : passwd error : " + user->_name);
            // 告知对端有问题
            TLV tlv;
            tlv.type = TASK_TYPE_LOGIN_SECTION2_RESP_ERROR;
            tlv.length = 0; // 不需要发送消息体了
            _conn->sendInLoop(tlv);
            cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
            return ;
        }

        // 密码正确
        LogManger::getInstance().info("UserLoginSection2::process() : passwd correct : " + user->_name);
        // 修改用户状态
        user->_status = USER_HAS_LOGIN;
        
        // 登录成功
        TLV tlv;
        tlv.type = TASK_TYPE_LOGIN_SECTION2_RESP_OK;
        tlv.length = 0; // 不需要发送消息体了
        _conn->sendInLoop(tlv);
        cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
        return ;
    }
    else  // _packet.type == TASK_TYPE_REGISTER2
    {
        // 注册用户 那么就需要更新数据库中的 密文字段
        // 需要先根据 连接指针TcpConnectionPtr 来确定是哪一个用户名
        string _userMangerName;
        User* user = UserManger::getInstance().getUser(_conn);
        _userMangerName = user->_name;

        // 根据名字查找数据库 然后更新密文段
        string sql = "SELECT * FROM user WHERE name = '" + _userMangerName + "'";
        vector<vector<string>> res = myclient.readOperationQuery(sql);
        if(res.size() == 0) {
            // 用户不存在
            // LogManger::getInstance().error("UserLoginSection2::process() : register failed, not in user : " + _userMangerName);
            // // 告知对端有问题
            // TLV tlv;
            // tlv.type = TASK_TYPE_REGISTER2_RESP_ERROR;
            // tlv.length = 0; // 不需要发送消息体了
            // _conn->sendInLoop(tlv);
            // cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
            // return ;
        }

        // 用户存在
        // 更新数据库中的 密文字段
        string sql2 = "UPDATE user SET encrypt = '" + passwd + "' WHERE name = '" + _userMangerName + "'";
        myclient.writeOperationQuery(sql2);

        // 更新UserManger中的密文 状态不必修改，依旧是未登录
        user->_encrypt = passwd;

        // 注册成功
        LogManger::getInstance().info("UserLoginSection2::process() : register success : " + _packet.msg);
        TLV tlv;
        tlv.type = TASK_TYPE_REGISTER2_RESP_OK;
        tlv.length = 0; // 不需要发送消息体了
        _conn->sendInLoop(tlv);
        cout << tlv.type << " : " << tlv.length << " : " << tlv.data << endl;
    }

}

void getSetting(string & hash, const char * passwd)
{
    // 只分离密文部分（hash）
    const char *p = passwd;
    int last_dollar = -1;
    for (int i = 0; p[i]; ++i) {
        if (p[i] == '$') last_dollar = i;
    }
    if (last_dollar != -1) {
        hash = passwd + last_dollar + 1;
    } else {
        hash = "";
    }
}


// 生成salt 8 位
char* GenRandomString(int length)  
{  
    int flag, i;  
    char* string;  
    srand((unsigned) time(NULL ));  
    if ((string = (char*) malloc(length)) == NULL )  
    {  
        printf("malloc failed!flag:14\n");  
        return NULL ;  
    }  
    for (i = 0; i < length+1; i++)  
    {  
        flag = rand() % 3;  
        switch (flag)  {  
            case 0:  string[i] = 'A' + rand() % 26;  break;  
            case 1:  string[i] = 'a' + rand() % 26;  break;  
            case 2:  string[i] = '0' + rand() % 10;  break;  
            default:  string[i] = 'x';  break;  
        }
    }
    string[length] = '\0';
    return string;  
} 