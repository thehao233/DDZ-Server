#include "TcpConnection.h"
#include "HttpRequest.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <cstring>
#include <netinet/in.h>
#include "sstream"
#include "RsaCrypto.h"
#include "Codec.h"
#include "Log.h"
#include "GameCommunication.h"
#include <iostream>

int TcpConnection::processRead(void* arg)
{
    Debug("开始接收数据了(基于读事件发送)....");
    TcpConnection* conn = static_cast<TcpConnection*>(arg);
    // 接收数据
    int socket = conn->m_channel->getSocket();
    int count = conn->m_readBuf->socketRead(socket);

//    Debug("接收到的请求数据: %s", conn->m_readBuf->data());
    if (count > 0)
    {
        /**接收到数据，开始处理 */
        Debug("接收到数据，开始处理.....");
        conn->m_gameComm->handleRequest(conn->m_readBuf);
    }
    else
    {
        // 和客户端断开连接
        conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
    }
//#ifndef MSG_SEND_AUTO
//    // 断开连接
//    conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
//#endif
    return 0;
}

int TcpConnection::processWrite(void* arg)
{
    Debug("开始发送数据了(基于写事件发送)....");
    TcpConnection* conn = static_cast<TcpConnection*>(arg);
    // 发送数据
    int count = conn->m_writeBuf->sendData(conn->m_channel->getSocket());
    if (count > 0)
    {
        // 判断数据是否被全部发送出去了
        if (conn->m_writeBuf->readableSize() == 0)
        {
            /** 发送完数据后开始监听读事件 */
            // 1. 不再检测写事件 -- 修改channel中保存的事件
            conn->m_channel->setCurrentEvent(FDEvent::ReadEvent);
            // 2. 修改dispatcher检测的集合 -- 添加任务节点
            conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
            Debug("数据发送完毕...................");
        }
    }
    return 0;
}

int TcpConnection::destroy(void* arg)
{
    TcpConnection* conn = static_cast<TcpConnection*>(arg);
    if (conn != nullptr)
    {
        delete conn;
    }
    return 0;
}

TcpConnection::TcpConnection(int fd, EventLoop* evloop)
{
    m_evLoop = evloop;
    m_readBuf = new Buffer(10240);
    m_writeBuf = new Buffer(10240);
    m_gameComm = new GameCommunication;

    Debug("设置回调函数....");
    auto writeFunc = std::bind(&TcpConnection::addWriteTask, this, std::placeholders::_1);
    auto deleteFunc = std::bind(&TcpConnection::addDeleteTask, this);

    m_gameComm->setCallback(writeFunc, deleteFunc);

    // 将非对称加密的公钥和签名后的公钥写入缓冲区
    processRsaSecretKey();
    m_name = "Connection-" + to_string(fd);
    // 设置写事件监听
    m_channel = new Channel(fd, FDEvent::WriteEvent, processRead,
                            processWrite, destroy, this);
    evloop->addTask(m_channel, ElemType::ADD);
}

TcpConnection::~TcpConnection()
{
    if (m_readBuf && m_readBuf->readableSize() == 0 &&
        m_writeBuf && m_writeBuf->readableSize() == 0)
    {
        delete m_readBuf;
        delete m_writeBuf;
        m_evLoop->freeChannel(m_channel);
    }

    if (m_gameComm)
    {
        delete m_gameComm;
    }

    Debug("连接断开, 释放资源, gameover, connName: %s", m_name.data());
}

void TcpConnection::processRsaSecretKey() {
    // 将非对称加密的公钥和签名后的公钥写入缓冲区
    ifstream ifs("public.pem");
    stringstream ss;
    // 将读缓冲区内的数据读取出来
    ss << ifs.rdbuf();
    string data = ss.str();

    // 将使用私钥对公钥进行数字签名
    RsaCrypto priKey("private.pem", RsaCrypto::PrivateKey);
    string signedData = priKey.sign(data);

    Message msg;
    msg.data1 = data;
    msg.data2 = signedData;
    msg.resCode = ResponseCode::RsaFenfa;

    // 对数据进行序列化
    Codec codec;
    string encodedData = codec.encodeMsg(&msg);

    m_writeBuf->appendDataPackage(encodedData);
}

void TcpConnection::addWriteTask(string msg) {
    Debug("添加写数据任务....");
    // 将数据写入到缓冲区
    m_writeBuf->appendDataPackage(msg);
#if 0
    // 通过反应堆发送
    // 监听写事件
    m_channel->setCurrentEvent(FDEvent::WriteEvent);
    m_evLoop->addTask(m_channel, ElemType::MODIFY);
#else
    // 直接发送
    m_writeBuf->sendData(m_channel->getSocket());
#endif
    Debug("数据已发送完成....");
}

void TcpConnection::addDeleteTask() {
    Debug("断开了和客户端的连接, connName = %s", m_name.data());
    m_evLoop->addTask(m_channel, ElemType::DELETE);
}
