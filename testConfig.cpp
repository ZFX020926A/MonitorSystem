// // #include "Configuration.hpp"
// // #include "LogManger.hpp"

// // #include <iostream>
// // #include <string>
// // using std::cout;
// // using std::endl;
// // using std::cin;
// // using std::string;

// // void test(string path){
// //     // 得到单例对象
// //     Configuration &configMesg = Configuration::getInstance();
// //     // 读取配置文件
// //     bool ret = configMesg.loadFromFile(path);
// //     if(ret == false)
// //     {
// //         cout << "load config message fail" << endl;
// //         return;
// //     }

// //     // 初始化日志模块
// //     LogManger::Init(configMesg.getValue("log_file"));

// //     string ip = configMesg.getValue("ip");
// //     string port = configMesg.getValue("port");
// //     string thread_num = configMesg.getValue("thread_num");
// //     string task_num = configMesg.getValue("task_num");
// //     string video = configMesg.getValue("video_path");
// //     string log = configMesg.getValue("log_file");

// //     cout << "ip = " << ip << endl;
// //     cout << "port = " << port << endl;
// //     cout << "thread_num = " << thread_num << endl;
// //     cout << "task_num = " << task_num << endl;
// //     cout << "video = " << video << endl;
// //     cout << "log = " << log << endl;

 
   
// // }

// // int main(int argc, char *argv[])
// // {
// //     string path = argv[1];
// //     test(path);
// //     return 0;
// // }

// // #include "SmartHomeMonitorServer.hpp"
// // #include "Configuration.hpp"
// // #include "LogManger.hpp"
// // #include <unistd.h>
// // #include <cstdlib>
// // #include <iostream>
// // using std::cout;
// // using std::endl;


// // int main(int argc, char const *argv[])
// // {
// //     string path = argv[1];
// //     Configuration &configMesg = Configuration::getInstance();
// //     configMesg.loadFromFile(path);
    
// //     LogManger::Init(configMesg.getValue("log_file"));

// //     SmartHomeMonitorServer server(configMesg.getValue("ip"), 
// //                                   configMesg.getValue("port"), 
// //                                   atoi(configMesg.getValue("thread_num").c_str()), 
// //                                   atoi(configMesg.getValue("task_num").c_str()));

// //     server.start();

// // 	return 0;
// // }


# if 0

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
    
    // 创建表
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

#endif 


// //  create DATABASE IF NOT EXISTS UserMesage;
// //                    主键                               用户名             MD5前缀+盐值      加密密文
// // "CREATE TABLE user(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(20), setting CHAR, encrypt CHAR)"; 
// // myclient.writeOperationQuery(sql1);

// // "INSERT INTO user(name, setting, encrypt) VALUES('用户名', 'md5前缀+盐值', '加密密文')";




# if 1

#include "SmartHomeMonitorServer.hpp"
#include "Configuration.hpp"
#include "LogManger.hpp"
#include "MySql.hpp"
#include "FFmpegVideo.hpp"


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


    // MySql myclient;
    // myclient.connect("8.148.74.118", "root", "1234", "UserMesage", 3306);
    
    // // 创建用户表
    // string sql1 = "CREATE TABLE IF NOT EXISTS user(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(20), setting CHAR(64), encrypt CHAR(128))"; 
    // myclient.writeOperationQuery(sql1);

    // // 创建设备表
    // string sql2 = "CREATE TABLE IF NOT EXISTS deviceTable(id INT PRIMARY KEY AUTO_INCREMENT, type INT, serial_no CHAR(128), channels INT, ip CHAR(24), rtsp CHAR(128), rtmp CHAR(128))";
    // myclient.writeOperationQuery(sql2);


    // rtsp字段后面会根据请求拼接上0/1，表示主码流或者副码流 两个频道
    // string sql3 = "INSERT INTO deviceTable(type, serial_no, channels, ip, rtsp, rtmp) VALUES(1, '09aaa54c30d06875d926a874617c06a5', 2, '192.168.105.222', 'rtsp://admin:admin@192.168.105.222/live/chn=0', 'rtmp://192.168.105.222:1935/hlsram/live0')";
    // myclient.writeOperationQuery(sql3);
    // INSERT INTO deviceTable(type, serial_no, channels, ip, rtsp, rtmp) VALUES(0, 'f6fdffe48c908deb0f4c3bd36c032e72', 2, '192.168.105.100', 'rtsp://admin:admin@192.168.105.100/live/chn=0', 'rtmp://192.168.105.100:1935/hlsram/live0')
    server.start();

    // string sql3 = "select * from user";
    // vector<vector<string>> res = myclient.readOperationQuery(sql3);
    // MySql::dump(res);
    // while (1)
    
    // string url = "rtsp://admin:admin@192.168.105.222/live/chn=0";
    // FFmpegVideo ffmpegvideo;
    // ffmpegvideo.openVideo();
    // ffmpegvideo.readFrame();        
    //ffmpegvideo.closeVideo();

	return 0;
}

#endif

    // string sql2 = "INSERT INTO user(name, setting, encrypt) VALUES('用户名', 'md5前缀+盐值', '加密密文')";
    // myclient.writeOperationQuery(sql2);