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

// //  create DATABASE IF NOT EXISTS UserMesage;
// //                    主键                               用户名             MD5前缀+盐值      加密密文
// // "CREATE TABLE user(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(20), setting CHAR, encrypt CHAR)"; 
// // myclient.writeOperationQuery(sql1);

// // "INSERT INTO user(name, setting, encrypt) VALUES('用户名', 'md5前缀+盐值', '加密密文')";




// #include <iostream>
// #include <map>
// #include <vector>
// #include <algorithm>
// #include <sstream>
// #include <iomanip>
// #include <openssl/md5.h>

// std::string mySecret = "f6fdffe48c908deb0f4c3bd36c032e72";

// // 计算MD5
// std::string md5(const std::string& str) {
//     unsigned char result[MD5_DIGEST_LENGTH];
//     MD5((unsigned char*)str.c_str(), str.size(), result);
//     std::ostringstream sout;
//     sout << std::hex;
//     for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
//         sout << std::setw(2) << std::setfill('0') << (int)result[i];
//     return sout.str();
// }

// // 参数处理
// std::map<std::string, std::string> md5Params(std::map<std::string, std::string> params) {
//     // t参数
//     time_t now = time(nullptr);
//     params["t"] = std::to_string(now);

//     // key排序
//     std::vector<std::string> keys;
//     for (const auto& kv : params) keys.push_back(kv.first);
//     keys.push_back("secret");
//     std::sort(keys.begin(), keys.end());

//     // 拼接字符串
//     std::string str;
//     for (const auto& key : keys) {
//         if (key == "token") continue;
//         if (!str.empty()) str += "&";
//         if (key == "secret") {
//             str += key + "=" + mySecret;
//         } else {
//             str += key + "=" + params[key];
//         }
//     }
//     params["token"] = md5(str);
//     std::cout << "md5: " << str << " " << params["token"] << std::endl;
//     return params;
// }

// // 使用样例
// void GetRecordList() {
//     std::map<std::string, std::string> args;
//     args["beginTime"] = "1753153239709";
//     args["endTime"] = "1753199999999";
//     args["channel"] = "1";
//     auto newArgs = md5Params(args);

//     std::ostringstream reqUrl;
//     //reqUrl << "http://192.168.105.222/xsw/tmpInfo?"
//     reqUrl << "http://192.168.105.222/xsw/api/record/list?"
//            << "beginTime=" << newArgs["beginTime"]
//            << "&endTime=" << newArgs["endTime"]
//            << "&channel=" << newArgs["channel"]
//            << "&t=" << newArgs["t"]
//            << "&token=" << newArgs["token"];
//     std::cout << "reqUrl: " << reqUrl.str() << std::endl;
// }

// int main() {
//     GetRecordList();
//     return 0;
// }



// #include <iostream>
// #include <sstream>
// #include <map>
// #include <vector>
// #include <algorithm>
// #include <curl/curl.h>
// #include <cjson/cJSON.h>

// // 回调函数用于接收响应数据
// size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
//     ((std::string*)userp)->append((char*)contents, size * nmemb);
//     return size * nmemb;
// }

// // 拼接URL参数
// std::string buildUrl(const std::string& base, const std::map<std::string, std::string>& params) {
//     std::ostringstream oss;
//     oss << base << "?";
//     bool first = true;
//     for (const auto& kv : params) {
//         if (!first) oss << "&";
//         oss << kv.first << "=" << kv.second;
//         first = false;
//     }
//     return oss.str();
// }

// int main() {
//     // 构造参数
//     std::map<std::string, std::string> params;
//     params["beginTime"] = "1753153239709";
//     params["endTime"] = "1753199999999";
//     params["channel"] = "0";
//     params["t"] = "1753154927";
//     params["token"] = "f6fdffe48c908deb0f4c3bd36c032e72";

//     std::string url = buildUrl("http://192.168.105.222/xsw/api/record/list", params);

//     CURL* curl = curl_easy_init();
//     std::string response;
//     if (curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
//         CURLcode res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//         }
//         curl_easy_cleanup(curl);
//     }

//     std::cout << "Response: " << response << std::endl;

//     // 用 cJSON 解析 JSON 返回
//     cJSON* root = cJSON_Parse(response.c_str());
//     if (!root) {
//         std::cerr << "JSON parse error" << std::endl;
//     } else {
//         cJSON* files = cJSON_GetObjectItem(root, "files");
//         if (files && cJSON_IsArray(files)) {
//             int size = cJSON_GetArraySize(files);
//             for (int i = 0; i < size; ++i) {
//                 cJSON* item = cJSON_GetArrayItem(files, i);
//                 cJSON* dur = cJSON_GetObjectItem(item, "dur");
//                 cJSON* name = cJSON_GetObjectItem(item, "name");
//                 if (dur && name) {
//                     std::cout << "dur: " << dur->valueint << ", name: " << (long long)name->valuedouble << std::endl;
//                 }
//             }
//         }
//         cJSON_Delete(root);
//     }

//     return 0;
// }

