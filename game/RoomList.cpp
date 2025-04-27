//
// Created by thehao on 2025/4/26.
//

#include "RoomList.h"
#include "Log.h"

RoomList *RoomList::getInstance() {
    static RoomList roomList;
    return &roomList;
}

void RoomList::addInfo(std::string roomname, std::string username, SendMsgCallback func) {

    // 对共享资源加锁
    std::unique_lock<std::mutex> locker(m_mutex);

    Debug("addInfo.......");
    auto it = m_roomMap.find(roomname);
    if (it == m_roomMap.end())
    {
        // 房间未存在
        Debug("房间未存在, 正在新建并添加数据....");
        UsersSendFuncMap usersMap = {{username, func}};
        m_roomMap[roomname] = usersMap;
        for (auto data:getUsersSendFuncMap(roomname))
        {
            Debug("roomname:%s, username:%s", roomname.c_str(), data.first.c_str());
        }
    }
    else
    {
        // 房间已存在
        Debug("房间存在, 正在添加数据....");
        UsersSendFuncMap &usersMap = m_roomMap[roomname];
        usersMap.insert(std::make_pair(username, func));
    }

}

UsersSendFuncMap RoomList::getUsersSendFuncMap(std::string roomname) {

    Debug("查询的roomname:%s", roomname.c_str());
    auto it = m_roomMap.find(roomname);
    if (it == m_roomMap.end())
    {
        // 没有该房间，返回空数据
        Debug("getUsersSendFuncMap, roomname:%s, 没有该房间，返回空数据", roomname.c_str());
        return UsersSendFuncMap();
    }

    return m_roomMap[roomname];
}
