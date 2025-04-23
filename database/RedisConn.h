//
// Created by thehao on 2025/4/23.
//

#ifndef DDZ_SERVER_REDISCONN_H
#define DDZ_SERVER_REDISCONN_H
#include <sw/redis++/redis.h>
#include <string>

class RedisConn {
public:

    RedisConn() = default;
    ~RedisConn();

    // 初始化环境
    bool initEnvironment();

    // 清空数据库信息
    void clear();

    // 存储RSA密钥对
    void saveRsaKey(std::string field, std::string value);
    // 获取RSA秘钥
    std::string getRsaKey(std::string field);

private:
    sw::redis::Redis *m_redis = nullptr;
};


#endif //DDZ_SERVER_REDISCONN_H
