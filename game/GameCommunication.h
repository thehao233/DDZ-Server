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

private:
    // 读取请求数据
    std::shared_ptr<Message> parseRequestData(Buffer* readBuffer);

    // 处理Aes秘钥分发
    void handleAesFenfa(std::shared_ptr<Message> requestMsg, Message& responseMsg);
    // 处理用户登录
    void handleUserRegister(std::shared_ptr<Message> requestMsg, Message& responseMsg);
    // 处理用户登录
    void handleUserLogin(std::shared_ptr<Message> requestMsg, Message& responseMsg);

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
