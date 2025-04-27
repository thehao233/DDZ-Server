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
    ~Room();

    // 随机加入房间
    std::string joinRoom(std::string username);
    // 加入指定房间
    bool joinRoom(std::string username, std::string roomname);

    // 获取用户分数
    int getPlayerScore(std::string roomname, std::string username);
    // 更新用户分数
    void updatePlayerScore(std::string roomname, std::string username, int score);

    // 获取用户当前所在的游戏房间
    std::string currentRoom(std::string username);
    // 获取房间内的人数
    int playerCount(std::string roomname);

private:
    std::string generateRandomRoom();

private:
    RedisConn *m_redisConn = nullptr;

    // 三人房间
    const std::string ThreePlayerRooms = "ThreePlayerRooms";
    // 两人房间
    const std::string TwoPlayerRooms = "TwoPlayerRooms";
    // 一人房间
    const std::string OnePlayerRooms = "OnePlayerRooms";

};


#endif //DDZ_SERVER_ROOM_H
