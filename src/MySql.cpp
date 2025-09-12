#include "MySql.hpp"
#include "LogManger.hpp"

#include <iostream>
#include <mysql/mysql.h>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::to_string;

MySql::MySql()
{
    // 初始化数据库连接
    MYSQL * pconn = mysql_init(&_conn);
    if(!pconn) {
        cout << "mysqlclient init error" << endl;
    }
}

MySql::~MySql()
{
    mysql_close(&_conn);
}

bool MySql::connect(const string & host, const string & user, const string & passwd, 
                    const string & db, unsigned short port)
{
    MYSQL *pconn = mysql_real_connect(&_conn, host.c_str(), user.c_str(), passwd.c_str(),
                                      db.c_str(), port, nullptr, 0);
    if(!pconn)
    {
        // 连接失败
        LogManger::getInstance().error("MySql connect error (" 
                                        + to_string(mysql_errno(&_conn)) +", " 
                                        + string(mysql_error(&_conn)) + ")");

        cout << "MySql connect error (" 
             << mysql_errno(&_conn) << ", " 
             << mysql_error(&_conn) << ")" << endl;

        return false;
    }
    
    // 连接成功
    LogManger::getInstance().info("MySql connect success");
    cout << "MySql connect success" << endl;
    return true;
}

//执行INSERT/UPDATE/DELTE语句
bool MySql::writeOperationQuery(const string & sql)
{
    int ret = mysql_real_query(&_conn, sql.c_str(), sql.size());
    if(ret != 0)
    {
        // 执行失败
        LogManger::getInstance().error("MySql writeOperationQuery error (" 
                                        + to_string(mysql_errno(&_conn)) +", " 
                                        + string(mysql_error(&_conn)) + ") ： " + sql);

        cout << "MySql writeOperationQuery error (" 
             << mysql_errno(&_conn) << ", " 
             << mysql_error(&_conn) << ")" << endl;

        return false;
    }
    // 执行成功
    LogManger::getInstance().info("MySql writeOperationQuery success ： " + sql);
    cout << "MySql writeOperationQuery success " << sql << endl;
    return true;
}

//执行select语句的查询
vector<vector<string>> MySql::readOperationQuery(const string & sql)
{
    int ret = mysql_real_query(&_conn, sql.c_str(), sql.size());
    if(ret != 0)
    {
        // 执行失败
        LogManger::getInstance().error("MySql readOperationQuery error (" 
                                        + to_string(mysql_errno(&_conn)) +", " 
                                        + string(mysql_error(&_conn)) + ") ： " + sql);

        cout << "MySql readOperationQuery error (" 
             << mysql_errno(&_conn) << ", " 
             << mysql_error(&_conn) << ")" << endl;

        return vector<vector<string>>();
    }

    // 获取结果集
    vector<vector<string>> res;
    MYSQL_RES * pres = mysql_store_result(&_conn);
    if(!pres)
    {
        // 获取结果集失败
        LogManger::getInstance().error("MySql readOperationQuery store_result error ("
                                        + to_string(mysql_errno(&_conn)) +", " 
                                        + string(mysql_error(&_conn)) + ") ： " + sql);

        cout << "MySql readOperationQuery store_result error (" 
             << mysql_errno(&_conn) << ", " 
             << mysql_error(&_conn) << ")" << endl;

        return vector<vector<string>>();
    }

    // 获取到结果集
    // 获取结果集的行数和列数
    int rows = mysql_num_rows(pres);
    if(rows == 0)
    {
        mysql_free_result(pres);
        LogManger::getInstance().info("MySql readOperationQuery success, but no data  " );
        cout << "MySql readOperationQuery success, but no data  " << endl;

        return vector<vector<string>>();
    }

    res.reserve(rows + 1);
    int cols = mysql_num_fields(pres);
    LogManger::getInstance().info("MySql readOperationQuery success, rows: " 
                                    + to_string(rows) + ", cols: " + to_string(cols));

    cout << "MySql readOperationQuery success, rows: " 
         << rows << ", cols: " << cols << endl;

    // 将列名放入结果集的第一行
    vector<string> colnames;
    colnames.reserve(cols);
    MYSQL_FIELD * fields = nullptr;
    while ((fields = mysql_fetch_field(pres)) != nullptr)
    {
        colnames.push_back(fields->name);
    }
    res.push_back(std::move(colnames));

    // 逐行获取结果集中的数据
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(pres)) != nullptr)
    {
        vector<string> curRow;
        curRow.reserve(cols);
        for(int i = 0; i < cols; ++i)
        {
            if(row[i]) 
            {
                curRow.push_back(string(row[i]));
            }
            else
            {
                curRow.push_back(string());
            }
        }
        res.push_back(std::move(curRow));
    }
    mysql_free_result(pres);
    return res;
}


//对结果集进行打印操作
void MySql::dump(const vector<vector<string>> & res)
{
    LogManger::getInstance().info("MySql dump result");
    for (auto & vec : res)
    {
        for(auto & filed : vec)
        {
            cout << filed << "\t";
        }
        cout << endl;   
    }
    cout << endl;
}