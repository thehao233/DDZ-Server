//
// Created by thehao on 2025/4/18.
//

#ifndef DDZ_SERVER_GAMECOMMUNICATION_H
#define DDZ_SERVER_GAMECOMMUNICATION_H
#include <memory>
#include "Codec.h"
#include "Buffer.h"
#include "AesCrypto.h"
#include <functional>
#include "MySqlConn.h"
#include "RedisConn.h"
#include "RoomList.h"

class GameCommunication {
public:
    using sendMsgCallback = function<void(string)>;
    using deleteConnCallback = function<void()>;

    GameCommunication();
    ~GameCommunication();

    // 对客户端请求进行处理
    void handleRequest(Buffer *readBuffer);

    // 设置数据发送和连接断开的回调函数
    void setCallback(sendMsgCallback func1, deleteConnCallback func2);

    // 向redis服务器 存储RSA密钥对
    void saveRsaKey(std::string field, std::string value);
    // 向redis服务器 获取RSA秘钥
    std::string getRsaKey(std::string field);

private:
    // 读取请求数据
    std::shared_ptr<Message> parseRequestData(Buffer* readBuffer);

    // 处理Aes秘钥分发
    void handleAesFenfa(std::shared_ptr<Message> requestMsg, Message& responseMsg);
    // 处理用户登录
    void handleUserRegister(std::shared_ptr<Message> requestMsg, Message& responseMsg);
    // 处理用户登录
    void handleUserLogin(std::shared_ptr<Message> requestMsg, Message& responseMsg);
    // 处理创建房间
    void handleCreateRoom(std::shared_ptr<Message> requestMsg, Message& responseMsg);
    // 处理加入房间
    void handleJoinRoom(std::shared_ptr<Message> requestMsg, Message& responseMsg);

    // 判断是否可以开始游戏
    /** 1. 如果不可以：设置向房间内所有用户通知人数信息的函数
     *  2. 如果可以：设置向房间内所有用户通知游戏开始的函数，并附带生成的牌组信息
     * */
    void readyForPlay(std::string roomname, std::string data);
    // 开始游戏
    /**
     * 1.生成卡牌信息
     * 2.将信息发送给用户们
     * */
    void startGame(std::string roomname, UsersSendFuncMap players);

    // 获取三个用户的出牌顺序
    std::string getPlayersOrder(std::string roomname);

private:
    sendMsgCallback m_sendMsgCallback;
    deleteConnCallback m_deleteConnCallback;

    // aes加密对象
    AesCrypto *m_aes = nullptr;

    // mysql连接
    MySqlConn *m_mysqlConn = nullptr;

    // redis连接
    RedisConn *m_redisConn = nullptr;

};


#endif //DDZ_SERVER_GAMECOMMUNICATION_H
