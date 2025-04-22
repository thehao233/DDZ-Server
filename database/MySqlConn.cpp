//
// Created by thehao on 2025/4/20.
//

#include <iostream>
#include "MySqlConn.h"

MySqlConn::MySqlConn() {
    // 初始化数据库连接
    m_conn = mysql_init(NULL);
    // 设置 utf8 编码
    mysql_set_character_set(m_conn, "utf8");
}

MySqlConn::~MySqlConn() {
    freeResult();
    // 释放数据库连接资源
    if (m_conn == nullptr) return;
    mysql_close(m_conn);
}

bool MySqlConn::connect(string user, string pwd, string dbName, string ip, unsigned short port) {
    // 连接数据库
    MYSQL* mysql = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), pwd.c_str(),
                                      dbName.c_str(), port, NULL, 0);
    return mysql != NULL;
}

bool MySqlConn::update(string sql) {
    // 增、删、改
    // 执行失败返回非0值
    int flag = mysql_query(m_conn, sql.c_str());
    return flag == 0;
}

bool MySqlConn::query(string sql) {
    // 先清空result
    freeResult();
    if (mysql_query(m_conn, sql.c_str()))
    {
        cout << "查询失败\n";
        return false;
    }
    // 将结果集保存到客户端
    m_result = mysql_store_result(m_conn);
    return true;
}

bool MySqlConn::next() {
    if (m_result == nullptr) return false;
    m_row = mysql_fetch_row(m_result);
    if (m_row == nullptr)
    {
        return false;
    }
    return true;
}

string MySqlConn::value(int index) {

    // 得到列数
    unsigned int num = mysql_num_fields(m_result);
    if (index < 0 || index >= num) return std::string();

    // 获取字段数据
    char *val = m_row[index];

    // 获取字段数据的长度，防止字段数据中出现\0，影响string转化后的结果正确性
    unsigned long *fetch_lens = mysql_fetch_lengths(m_result);
    unsigned long length = fetch_lens[index];

    return std::string(val, length);
}

void MySqlConn::transaction() {
    // auto_mode: 1-自动  0-手动
    mysql_autocommit(m_conn, 0);
}

void MySqlConn::commit() {
    mysql_commit(m_conn);
    mysql_autocommit(m_conn, 1);
}

void MySqlConn::rollback() {
    mysql_rollback(m_conn);
    mysql_autocommit(m_conn, 1);
}

void MySqlConn::freeResult() {
    if (m_result == nullptr) return;
    mysql_free_result(m_result);
    m_result = NULL;
}

void MySqlConn::refreshAliveTime()
{
    m_alivetime = steady_clock::now();
}

long long MySqlConn::getAliveTime() const
{
    nanoseconds res = steady_clock::now() - m_alivetime;
    milliseconds millSec = duration_cast<milliseconds>(res);
    return millSec.count();
}

