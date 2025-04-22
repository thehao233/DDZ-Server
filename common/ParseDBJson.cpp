//
// Created by thehao on 2025/4/22.
//

#include <fstream>
#include <cassert>
#include "ParseDBJson.h"
#include "Log.h"

ParseDBJson::ParseDBJson(std::string fileName) {

    Debug("开始解析json文件....");
    // 打开文件
    std::ifstream ifs(fileName);
    assert(ifs.is_open());

    // 获取json对象
    Json::Reader rd;
    rd.parse(ifs, m_dataBases);
    assert(m_dataBases.isObject());
    Debug("解析完成....");
}

std::shared_ptr<DBInfo> ParseDBJson::getDataBaseInfo(ParseDBJson::DBType type) {

    std::string DBName = type == DBType::Mysql ? "mysql" : "redis";

    // 注意 共享指针的初始化需要为其指定 待检测的内存
    std::shared_ptr<DBInfo> info(new DBInfo);
    Json::Value db = m_dataBases[DBName];

    Debug("开始获取json数据....");
    info->ip = db["ip"].asString();
    info->port = db["port"].asInt();

    if (type == DBType::Mysql)
    {
        info->user = db["user"].asString();
        info->password = db["password"].asString();
        info->dbname = db["dbname"].asString();
    }
    Debug("获取完成....");

    return info;
}
