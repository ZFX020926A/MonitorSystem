#ifndef __MYSQL_H__
#define __MYSQL_H__

#include <mysql/mysql.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class MySql
{
public:
    MySql();
    ~MySql();

    // 连接数据库
    bool connect(const string & host, const string & user, const string & passwd, 
                 const string & db, unsigned short port);
                 
    bool writeOperationQuery(const string & sql);

    // 打印查询结果
    static void dump(const vector<vector<string>> & res);

    // 返回二维数组，表示查询结果
    vector<vector<string>> readOperationQuery(const string & sql);

private:
    // 初始化数据库连接
    void init();

private:
    MYSQL _conn;
};


#endif // !__MYSQL_H__