//
// Created by thehao on 2025/4/24.
//

#ifndef DDZ_SERVER_ROOM_H
#define DDZ_SERVER_ROOM_H


#include <string>
#include "RedisConn.h"
#include <sw/redis++/redis.h>

class Room : public RedisConn{

public:

    Room();

    // 自动选取房间加入
    std::string joinRoom(std::string username);
    // 加入指定房间
    bool joinRoom(std::string username, std::string roomname);

private:
    RedisConn *m_redisConn = nullptr;

};


#endif //DDZ_SERVER_ROOM_H
