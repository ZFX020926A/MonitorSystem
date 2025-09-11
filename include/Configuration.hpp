#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <map>

using std::string;
using std::map;

class Configuration
{
public:
    // 从文件加载配置
    bool loadFromFile(const string &filename);

    // 根据键获取对应的值
    string getValue(const string &key);

    // 获取单例对象
    static Configuration &getInstance();
private:
    // 构造函数和析构函数私有化
    Configuration()
    {}
    ~Configuration()
    {}

    // 删除拷贝构造函数 和 赋值运算符函数
    Configuration(const Configuration &) = delete;
    Configuration &operator = (const Configuration &) = delete;

private:
    // 用来存储配置文件中的键值对
    map<string, string> _configMap; 
   
};

#endif // !__CONFIGURATION_H__