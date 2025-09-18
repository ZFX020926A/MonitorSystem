#include "cameraPTZ.hpp"

#include "LogManger.hpp"

#include <iostream>
#include <unistd.h> 
#include <string>

#include <openssl/md5.h>
#include <sstream>
#include <iomanip>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::ostringstream;


cameraPTZ::cameraPTZ(TcpConnectionPtr conn, const Packet& packet)
: _conn(conn)
, _packet(packet)
{
}

cameraPTZ::~cameraPTZ()
{
}

//上         u  
//下         d  
//左         l  
//右         r  
//左上       1  
//右上       2  
//左下       3  
//右下       4       
//停止转动   s 

// 生成MD5
string md5(const string& str) {
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)str.c_str(), str.size(), result);
    std::ostringstream sout;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        sout << std::hex << std::setw(2) << std::setfill('0') << (int)result[i];
    return sout.str();
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void cameraPTZ::run(const string &ptzCommand)
{
    map<string, string> _params = 
    {
        {"channelId", "1"},
        {"value", ptzCommand},
        {"secret", "f6fdffe48c908deb0f4c3bd36c032e72"},
        {"t", "1673255651"}
    };

    ostringstream paramStream;
    bool first = true;
    for (const auto& kv : _params) {
        if (!first) paramStream << "&";
        paramStream << kv.first << "=" << kv.second;
        first = false;
    }

    string paramStr = paramStream.str();

    // 生成token
    string token = md5(paramStr);

    ostringstream urlStream;
    urlStream << "http://192.168.105.222/xsw/api/ptz/control?";
    first = true;
    for (const auto& kv : _params) {
        if (kv.first == "secret") continue;
        if (!first) urlStream << "&";
        urlStream << kv.first << "=" << kv.second;
        first = false;
    }
    urlStream << "&token=" << token;

    cout << "MD5 token: " << token << endl;
    cout << "最终URL: " << urlStream.str() << endl;
    LogManger::getInstance().info("cameraPTZ::run() : MD5 tocken " + token);
    LogManger::getInstance().info("cameraPTZ::run() : Sending PTZ command with URL: " + urlStream.str());

    // 解析返回的字符串
    string url = urlStream.str();
    CURL* curl = curl_easy_init();
    string response;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            LogManger::getInstance().error("cameraPTZ::run() : curl_easy_perform() failed: " + string(curl_easy_strerror(res)));
        }
        curl_easy_cleanup(curl);
    }
    if (!response.empty()) {
        try {
            json j = json::parse(response);
            string msg = j["msg"];
            int code = j["code"];
            cout << "msg: " << msg << endl;
            cout << "code: " << code << endl;
        } catch (...) {
            cerr << "JSON解析失败，原始返回: " << response << endl;
            LogManger::getInstance().error("cameraPTZ::run() : JSON parse failed, original response: " + response);
        }
    } else {
        cerr << "未收到服务器返回" << endl;
        LogManger::getInstance().error("cameraPTZ::run() : no response received from server");
    }
}


void cameraPTZ::process()
{
    cout << "cameraPTZ::process()" << endl;
    cout << "Received PTZ command: " << _packet.msg << endl;

    // 解析_packet.msg 获取具体的PTZ指令
    string ptzCommand = _packet.msg; // 假设msg中存储的是PTZ指令

    // 执行控制命令
    run(ptzCommand);
    usleep(100000); // sleep for 0.1 seconds (100,000 microseconds)
    run("s"); // 停止命令
    LogManger::getInstance().info("cameraPTZ::process() : Executed PTZ command: " + ptzCommand);
}

