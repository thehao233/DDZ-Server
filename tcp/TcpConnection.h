#pragma once
#include "EventLoop.h"
#include "Buffer.h"
#include "Channel.h"
#include "GameCommunication.h"

//#define MSG_SEND_AUTO

class TcpConnection
{
public:
    TcpConnection(int fd, EventLoop* evloop);
    ~TcpConnection();

    static int processRead(void* arg);
    static int processWrite(void* arg);
    static int destroy(void* arg);

    // 将非对称加密的公钥和签名后的公钥写入缓冲区
    void processRsaSecretKey();

    // 发送数据
    void addWriteTask(string msg);
    // 断开连接
    void addDeleteTask();

private:
    string m_name;
    EventLoop* m_evLoop;
    Channel* m_channel;
    Buffer* m_readBuf;
    Buffer* m_writeBuf;

    // 游戏通信对象
    GameCommunication* m_gameComm;
};