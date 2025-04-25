//
// Created by thehao on 2025/4/23.
//

#include "RedisConn.h"
#include "Log.h"

bool RedisConn::initEnvironment() {

    // 加载json文件

    // 连接redis服务器
    std::string uri = "tcp://127.0.0.1:6379";
    m_redis = new sw::redis::Redis(uri);
    bool flag = m_redis->ping() == "PONG";
    if (flag)
    {
        Debug("成功连接Redis服务器.....");
        return true;
    }

    return false;
}

void RedisConn::saveRsaKey(std::string field, std::string value) {
    // 向名为 RsaKey 的hash表 添加 field:value 对
    m_redis->hset("RsaKey", field, value);
}

std::string RedisConn::getRsaKey(std::string field) {
    auto data = m_redis->hget("RsaKey", field);

    if (data.has_value())
    {
        return data.value();
    }

    return std::string();

}

void RedisConn::clear() {
    m_redis->flushdb();
}

RedisConn::~RedisConn() {
    if (m_redis)
    {
        delete m_redis;
    }
}

RedisConn::RedisConn() {

    // 连接redis服务器
    bool flag = initEnvironment();
    assert(flag);
}
