#ifndef __LOGMANGER_H__
#define __LOGMANGER_H__

// 此类用来初始化和管理日志系统

#include <log4cpp/Category.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <string>

using std::string;
using log4cpp::Category;
using log4cpp::PatternLayout;
using log4cpp::RollingFileAppender;
using log4cpp::Priority;


class LogManger
{

public:
    static void Init(const string &path);
    static LogManger& getInstance();
    void info(const string& msg);
    void error(const string& msg);
    // 可扩展更多日志级别

private:
    LogManger(const string &path);
    ~LogManger();
    LogManger(const LogManger&) = delete;
    LogManger& operator=(const LogManger&) = delete;

    Category& _logger;
    static LogManger * _LogInstance;
};

#endif // !__LOGMANGER_H__