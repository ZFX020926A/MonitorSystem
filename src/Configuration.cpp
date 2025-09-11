#include "Configuration.hpp"
#include "LogManger.hpp"

#include <fstream>
#include <sstream>

using std::ifstream;
using std::istringstream;

    // 从文件加载配置 到map中
bool Configuration::loadFromFile(const string &filenamePath)
{
    ifstream filefd(filenamePath);
    if(filefd.is_open() == false)
    {
        // 打开文件失败
        return false;
    }

    // 记录读取到的每一行
    string line;
    while(getline(filefd, line))
    {
        // 读取到一行
        istringstream iss(line);
        
        // 记录读取到的 配置信息 --> 键值对
        string key, value;
        if(iss >> key >> value)
        {
            // 将读取到的内容 存储到_configMap中
            _configMap[key] = value;
        }
    }
    return true;
}

    // 根据键获取对应的值
string Configuration::getValue(const string &key)
{
    auto value = _configMap.find(key);
    if(value != _configMap.end())
    {
        return value->second;
    }
    // 没有找到对应的key 则返回当前目录
    return "./";  
}

    // 获取单例对象
Configuration &Configuration::getInstance()
{
    static Configuration _instance;
    return _instance;
}



