//
// Created by thehao on 2025/4/24.
//

#include <iostream>
#include <random>
#include "Room.h"
#include "Log.h"

Room::Room() {
    m_redisConn = new RedisConn;
}

Room::~Room() {
    if (m_redisConn)
    {
        delete m_redisConn;
    }
}

std::string Room::joinRoom(std::string username) {

    std::string roomname = "";
    auto redis = m_redisConn->getRedis();

    // 首先判断两人房间
    if (redis->scard(TwoPlayerRooms) > 0)
    {
        roomname = redis->srandmember(TwoPlayerRooms).value();
    }
    // 判断一人房间
    else if (redis->scard(OnePlayerRooms) > 0)
    {
        roomname = redis->srandmember(OnePlayerRooms).value();
    }
    // 随机创建一个房间
    else
    {
        roomname = generateRandomRoom();
    }

    // 加入指定的房间
    joinRoom(username, roomname);

    return roomname;
}

bool Room::joinRoom(std::string username, std::string roomname) {

    auto redis = m_redisConn->getRedis();
    // 判断房间是否满员
    if (redis->zcard(roomname) >= 3)
    {
        Debug("房间满员，加入失败....");
        return false;
    }

    // 加入房间
    Debug("joinRoom 房间名:%s", roomname.c_str());
    if (!redis->exists(roomname))
    {
        // 不存在该房间 则 创建并加入
        redis->sadd(OnePlayerRooms, roomname);
        Debug("移入1用户房间......");
    }
    else if (redis->sismember(OnePlayerRooms, roomname))
    {
        redis->smove(OnePlayerRooms, TwoPlayerRooms, roomname);
        Debug("移入2用户房间......");
    }
    else if (redis->sismember(TwoPlayerRooms, roomname))
    {
        redis->smove(TwoPlayerRooms, ThreePlayerRooms, roomname);
        Debug("移入3用户房间......");
    }
    else
    {
        // 异常情况，直接退出
        assert(false);
    }
    // 存储信息       -> username1
    //      roomname -> username2
    //               ->username3
    // 将信息存入到房间中
    redis->zadd(roomname, username, 0);

    // 将 用户名-房间名 映射保存起来
    redis->hset("User2Room", username, roomname);
    return true;
}

std::string Room::generateRandomRoom() {

    // 创建随机设备对象，用于获取随机种子
    std::random_device rd;
    // 创建 mt19937 模块（梅森旋转算法），并用随机设备初始化种子
    std::mt19937 gen(rd());
    // 定义均匀分布的整数范围为 100000 - 999999
    std::uniform_int_distribution<> dis(100000, 999999);
    // 生成六位随机数
    int sixDigitRandom = dis(gen);
    // 设置房间名
    /** 这一步可能需要判断房间名是否已存在*/
    std::string roomname = std::to_string(sixDigitRandom);

    return roomname;
}

int Room::getPlayerScore(std::string roomname, std::string username) {
    auto redis = m_redisConn->getRedis();
    auto score = redis->zscore(roomname, username);
    return score.value();
}

void Room::updatePlayerScore(std::string roomname, std::string username, int score) {
    auto redis = m_redisConn->getRedis();
    redis->zadd(roomname, username, score);
}

std::string Room::currentRoom(std::string username) {
    auto redis = m_redisConn->getRedis();
    auto oldRoomname = redis->hget("User2Room", username);
    if (oldRoomname.has_value())
    {
        return oldRoomname.value();
    }
    return std::string();
}

int Room::playerCount(std::string roomname) {
    auto redis = m_redisConn->getRedis();
    return redis->zcard(roomname);
}
