#include "Configuration.hpp"
#include "LogManger.hpp"

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::cin;
using std::string;

void test(string path){
    // 得到单例对象
    Configuration &configMesg = Configuration::getInstance();
    // 读取配置文件
    bool ret = configMesg.loadFromFile(path);
    if(ret == false)
    {
        cout << "load config message fail" << endl;
        return;
    }

    // 初始化日志模块
    LogManger::Init(configMesg.getValue("log_file"));

    string ip = configMesg.getValue("ip");
    string port = configMesg.getValue("port");
    string thread_num = configMesg.getValue("thread_num");
    string task_num = configMesg.getValue("task_num");
    string video = configMesg.getValue("video_path");
    string log = configMesg.getValue("log_file");

    cout << "ip = " << ip << endl;
    cout << "port = " << port << endl;
    cout << "thread_num = " << thread_num << endl;
    cout << "task_num = " << task_num << endl;
    cout << "video = " << video << endl;
    cout << "log = " << log << endl;

    // std::string ip = configMesg.getValue("ip");
    // std::string port = configMesg.getValue("port");
    // size_t threadNum = std::stoi(configMesg.getValue("threadNum"));
    // size_t taskQueueSize = std::stoi(configMesg.getValue("taskQueueSize"));

    // // 启动服务器
    // SmartHomeMonitorServer server(ip, port, threadNum, taskQueueSize);
    // server.shmsStart();

    // // 主线程阻塞，或循环等待
    // cout << "Server is running. Press Enter to exit..." << endl;
    // cin.get();

    // server.shmsStop();
    // LogManger::getInstance().info("Server stopped.");
   
}

int main(int argc, char *argv[])
{
    string path = argv[1];
    test(path);
    return 0;
}
