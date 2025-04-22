//
// Created by thehao on 2025/4/22.
//

#ifndef DDZ_SERVER_PARSEDBJSON_H
#define DDZ_SERVER_PARSEDBJSON_H
#include <string>
#include <json/json.h>
#include <memory>

struct DBInfo{
    std::string ip;
    unsigned short port;
    std::string user;
    std::string password;
    std::string dbname;
};

class ParseDBJson {

public:
    enum DBType{Mysql, Redis};
    ParseDBJson(std::string fileName = "../config/DBConfig.json");

    // 获取数据
    std::shared_ptr<DBInfo> getDataBaseInfo(DBType type);

private:
    Json::Value m_dataBases;
};


#endif //DDZ_SERVER_PARSEDBJSON_H
