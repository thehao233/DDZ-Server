//
// Created by thehao on 2025/4/20.
//

#ifndef MYSQL_CONNPOOL_MySqlConn_H
#define MYSQL_CONNPOOL_MySqlConn_H
#include <string>
#include <mysql.h>
#include <chrono>

using namespace std;
using namespace chrono;

class MySqlConn {
public:
    // 初始化数据库连接
    MySqlConn();
    // 释放数据库连接
    ~MySqlConn();
    // 连接数据库
    bool connect(string user, string pwd, string dbName, string ip, unsigned short port = 3306);
    // 更新数据库：insert,update,delete
    bool update(string sql);
    // 查询数据库
    bool query(string sql);
    // 遍历查询得到的结果集
    // 用于从mysql存储的result集合中获取一行数据
    bool next();
    // 得到结果集中的字段值
    // 通过fetch的索引值获取对应的值
    string value(int index);


    // 刷新一下连接的起始的空闲时间点
    void refreshAliveTime();
    // 返回存活的时间
    long long getAliveTime()const;
    // 事务操作
    void transaction();
    // 提交事务
    void commit();
    // 事务回滚
    void rollback();
private:
    void freeResult();

private:
    MYSQL *m_conn = NULL;
    MYSQL_RES *m_result = NULL;
    MYSQL_ROW m_row = NULL;
    steady_clock::time_point m_alivetime;              // 记录进入空闲状态后的起始存活时间
};

#endif //DDZ_SERVER_MySqlConn_H
