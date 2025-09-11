#include "LogManger.hpp"

LogManger * LogManger::_LogInstance = nullptr;

void LogManger::Init(const string &path)
{
    if(nullptr == _LogInstance)
    {
        // 创建一个指向堆空间的 单例对象 参数时获取到的存储 log位置
        _LogInstance = new LogManger(path);
    }
}

LogManger::LogManger(const string &path)
: _logger(Category::getRoot())
{
    // 设置日志记录布局
    auto *layout = new PatternLayout();
    layout->setConversionPattern("%d %c [%p] %m%n");

    // 设置日志输出器
    auto *appender = new RollingFileAppender("rollingfile", path, 100 * 1024 * 1024, 9);

    // 输出器绑定布局
    appender->setLayout(layout);

    // 设置目的地
    _logger.addAppender(appender);

    // 设置过滤等级
    _logger.setPriority(Priority::INFO);
}

LogManger::~LogManger()
{
    // 回收资源
    Category::shutdown();
    delete _LogInstance;
    _LogInstance = nullptr;
}

LogManger& LogManger::getInstance()
{
    if(nullptr == _LogInstance)
    {
        throw std::runtime_error("LogManger is not initialized. Call LogManger::Init(path) first.");
    }
    return *_LogInstance;  
}

void LogManger::info(const string& msg)
{
    // 写入INFO级别日志
    _logger.info(msg);
}

void LogManger::error(const string& msg)
{
    // 写入ERROR级别日志
    _logger.error(msg);
}

