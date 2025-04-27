//
// Created by thehao on 2025/4/26.
//

#ifndef DDZ_SERVER_ROOMLIST_H
#define DDZ_SERVER_ROOMLIST_H
#include <map>
#include <string>
#include <functional>
#include <mutex>

using SendMsgCallback = std::function<void(std::string)>;
using UsersSendFuncMap = std::map<std::string, SendMsgCallback>;

// 单例类 - 用于存储房间中每个用户的通讯方式
class RoomList {

public:

    RoomList(const RoomList& other) = delete;
    RoomList operator=(const RoomList& other) = delete;

    static RoomList *getInstance();
    // 添加 房间 - 用户 - 通信函数 数据
    void addInfo(std::string roomname, std::string username, SendMsgCallback func);
    // 获取指定房间中 用户 - 通信函数
    UsersSendFuncMap getUsersSendFuncMap(std::string roomname);

private:
    RoomList() = default;

private:
    // m_roomMap为多线程共享资源，需要进行加锁
    std::mutex m_mutex;

    // 每个房间中，每个用户的通信函数
    std::map<std::string, UsersSendFuncMap> m_roomMap;

};


#endif //DDZ_SERVER_ROOMLIST_H
