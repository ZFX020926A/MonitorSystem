// #include "Configuration.hpp"
// #include "LogManger.hpp"

// #include <iostream>
// #include <string>
// using std::cout;
// using std::endl;
// using std::cin;
// using std::string;

// void test(string path){
//     // 得到单例对象
//     Configuration &configMesg = Configuration::getInstance();
//     // 读取配置文件
//     bool ret = configMesg.loadFromFile(path);
//     if(ret == false)
//     {
//         cout << "load config message fail" << endl;
//         return;
//     }

//     // 初始化日志模块
//     LogManger::Init(configMesg.getValue("log_file"));

//     string ip = configMesg.getValue("ip");
//     string port = configMesg.getValue("port");
//     string thread_num = configMesg.getValue("thread_num");
//     string task_num = configMesg.getValue("task_num");
//     string video = configMesg.getValue("video_path");
//     string log = configMesg.getValue("log_file");

//     cout << "ip = " << ip << endl;
//     cout << "port = " << port << endl;
//     cout << "thread_num = " << thread_num << endl;
//     cout << "task_num = " << task_num << endl;
//     cout << "video = " << video << endl;
//     cout << "log = " << log << endl;

 
   
// }

// int main(int argc, char *argv[])
// {
//     string path = argv[1];
//     test(path);
//     return 0;
// }

// #include "SmartHomeMonitorServer.hpp"
// #include "Configuration.hpp"
// #include "LogManger.hpp"
// #include <unistd.h>
// #include <cstdlib>
// #include <iostream>
// using std::cout;
// using std::endl;


// int main(int argc, char const *argv[])
// {
//     string path = argv[1];
//     Configuration &configMesg = Configuration::getInstance();
//     configMesg.loadFromFile(path);
    
//     LogManger::Init(configMesg.getValue("log_file"));

//     SmartHomeMonitorServer server(configMesg.getValue("ip"), 
//                                   configMesg.getValue("port"), 
//                                   atoi(configMesg.getValue("thread_num").c_str()), 
//                                   atoi(configMesg.getValue("task_num").c_str()));

//     server.start();

// 	return 0;
// }

#include "SmartHomeMonitorServer.hpp"
#include "Configuration.hpp"
#include "LogManger.hpp"
#include "MySql.hpp"


#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char const *argv[])
{
    string path = argv[1];

    Configuration &configMesg = Configuration::getInstance();
    // 读取配置文件
    configMesg.loadFromFile(path);
   
    SmartHomeMonitorServer server(configMesg.getValue("ip"), 
                                configMesg.getValue("port"), 
                                atoi(configMesg.getValue("thread_num").c_str()), 
                                atoi(configMesg.getValue("task_num").c_str()));

    

    // 初始化日志模块
    LogManger::Init(configMesg.getValue("log_file"));

    MySql myclient;
    myclient.connect("8.148.74.118", "root", "1234", "UserMesage", 3306);
    // 创建
    string sql1 = "CREATE TABLE IF NOT EXISTS user(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(20), setting CHAR(64), encrypt CHAR(128))"; 
    myclient.writeOperationQuery(sql1);

    server.start();
    // while (1)
    // {
    //     /* code */
    // }
    

    // string sql2 = "INSERT INTO user(name, setting, encrypt) VALUES('用户名', 'md5前缀+盐值', '加密密文')";
    // myclient.writeOperationQuery(sql2);
    string sql3 = "select * from user";
    vector<vector<string>> res = myclient.readOperationQuery(sql3);
    MySql::dump(res);

	return 0;
}

//  create DATABASE IF NOT EXISTS UserMesage;
//                    主键                               用户名             MD5前缀+盐值      加密密文
// "CREATE TABLE user(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(20), setting CHAR, encrypt CHAR)"; 
// myclient.writeOperationQuery(sql1);

// "INSERT INTO user(name, setting, encrypt) VALUES('用户名', 'md5前缀+盐值', '加密密文')";
