//
// Created by thehao on 2025/4/18.
//

#include <netinet/in.h>
#include <cassert>
#include "GameCommunication.h"
#include "Hash.h"
#include "Log.h"
#include "RsaCrypto.h"
#include "ParseDBJson.h"


std::shared_ptr<Message> GameCommunication::parseRequestData(Buffer* readBuffer) {

    string data = readBuffer->data(sizeof(int));
    int length = *(int*)data.data();    // 大端长度
    length = ntohl(length);
    data = readBuffer->data(length);

    // 如果已完成aes分发，则使用aes秘钥对数据进行解密
    if (m_aes != nullptr)
    {

        data = m_aes->deCrypto(data);
        Debug("aes解密完成....");

    }

    // 对数据进行反序列化
    Codec code;
    shared_ptr<Message> msg = code.decodeMsg(data);

    Debug("数据接收完成......");
    return msg;
}

void GameCommunication::handleRequest(Buffer* readBuffer) {

    // 读取请求数据
    std::shared_ptr<Message> requestMsg = parseRequestData(readBuffer);

    // 响应数据
    Message responseMsg;

    switch (requestMsg->reqCode)
    {
        case RequestCode::UserLogin:
            Debug("开始处理用户登录........");
            handleUserLogin(requestMsg, responseMsg);
            break;
        case RequestCode::Register:
            Debug("开始处理用户注册........");
            handleUserRegister(requestMsg, responseMsg);
            break;
        case RequestCode::AesFenfa:
            Debug("开始处理Aes分发.........");
            handleAesFenfa(requestMsg, responseMsg);
            break;
        default:
            break;
    }

    // 对返回数据进行序列化编码
    Codec codec;
    string encodedMsg = codec.encodeMsg(&responseMsg);
    // 返回响应数据
    sendMsgCallback sendMsg = m_sendMsgCallback;
    Debug("回复给客户端的数据: %s, size = %d, status: %d", encodedMsg.data(), encodedMsg.size(), responseMsg.resCode);
    sendMsg(encodedMsg);
}

void GameCommunication::handleAesFenfa(std::shared_ptr<Message> requestMsg, Message& responseMsg) {

    // 公钥加密后的Aes数据
    string enCryptoedAesKey = requestMsg->data1;
    // 哈希计算后的Aes数据
    string hashedAesKey = requestMsg->data2;

    // 获取非对称加密秘钥
    string priKeyStr = m_redisConn->getRsaKey("PrivateKey");
    RsaCrypto rsa;
    rsa.parseStringToKey(priKeyStr, RsaCrypto::KeyType::PrivateKey);

//    RsaCrypto rsa("private.pem", RsaCrypto::KeyType::PrivateKey);
    //  使用秘钥对数据进行解密
    string aesKey = rsa.deCrypto(enCryptoedAesKey);

    // 校验aes秘钥
    Hash hs(HashType::Sha256);
    hs.addData(aesKey);
    string realHashedAesKey = hs.result();
    bool flag = (hashedAesKey == realHashedAesKey);

//    Message responseMsg;
    if (flag)
    {
        Debug("Aes秘钥验证成功.......");
        responseMsg.resCode = ResponseCode::AesVerifyOK;
        m_aes = new AesCrypto(AesCrypto::AES_CBC_256, aesKey);
    }
    else
    {
        Debug("Aes秘钥验证失败.......");
        responseMsg.resCode = ResponseCode::Failed;
        responseMsg.data1 = "Aes秘钥验证失败.......";
    }
}

void
GameCommunication::setCallback(GameCommunication::sendMsgCallback func1, GameCommunication::deleteConnCallback func2)
{
    m_sendMsgCallback = func1;
    m_deleteConnCallback = func2;
}

GameCommunication::~GameCommunication() {

    if (m_mysqlConn != nullptr)
    {
        delete m_mysqlConn;
    }

    if (m_aes != nullptr)
    {
        delete m_aes;
    }

    if (m_redisConn)
    {
        delete m_redisConn;
    }
}

void GameCommunication::handleUserLogin(std::shared_ptr<Message> requestMsg, Message& responseMsg) {

    // 获取用户信息
    string username = requestMsg->username;
    string pwd = requestMsg->data1;

    // 设置查询语句
    char sql[1024];
    sprintf(sql,
            "select * from `user` where name = '%s' and passwd = '%s' and (select count(*) from `information` where name = '%s' and status = 0);",
            username.c_str(), pwd.c_str(), username.c_str());

    std::cout << "handleUserLogin 的 sql为：" << sql << std::endl;

    // 进行查询
    bool flag = m_mysqlConn->query(sql);
    if (flag && m_mysqlConn->next())
    {
        // 用户存在并未登录
        m_mysqlConn->transaction();
        sprintf(sql,
                "update `information` set status = 1 where name = '%s';",
                username.c_str());
        bool is_update = m_mysqlConn->update(sql);
        if (is_update)
        {
            m_mysqlConn->commit();
            responseMsg.resCode = ResponseCode::LoginOK;
        }
        else
        {
            m_mysqlConn->rollback();
            responseMsg.resCode = ResponseCode::Failed;
            responseMsg.data1 = "用户登录状态更新失败...";
        }
    }
    else
    {
        // 用户不存在 或 已经登录
        responseMsg.resCode = ResponseCode::Failed;
        responseMsg.data1 = "用户不存在 或 已经登录...";
    }
}

void GameCommunication::handleUserRegister(std::shared_ptr<Message> requestMsg, Message &responseMsg) {

    // 获取用户信息
    string username = requestMsg->username;
    string pwd = requestMsg->data1;
    string phone = requestMsg->data2;

    Debug("插入的 用户名：%s  密码：%s  手机号：%s", username.c_str(), pwd.c_str(), phone.c_str());

    // 设置查询语句
    char sql[1024];
    sprintf(sql, "select * from `user` where name = '%s';", username.c_str());

    // 进行查询
    bool flag = m_mysqlConn->query(sql);
    if (flag && m_mysqlConn->next())
    {
        // 该用户已存在
        responseMsg.resCode = ResponseCode::Failed;
        responseMsg.data1 = "用户名已存在...";
    }
    else
    {
        // 将用户信息存储到数据库中
        m_mysqlConn->transaction();
        // 为user表插入新数据
        sprintf(sql,
                "insert into `user` (name, passwd, phone, date) values ('%s', '%s', '%s', now());",
                username.c_str(), pwd.c_str(), phone.c_str());
        bool is_insert1 = m_mysqlConn->update(sql);

        // 为information表插入新数据
        sprintf(sql,
                "insert into `information` (name, score, status) values ('%s', 0, 0);",
                username.c_str());
        bool is_insert2 = m_mysqlConn->update(sql);

        if (is_insert1 && is_insert2)
        {
            m_mysqlConn->commit();
            responseMsg.resCode = ResponseCode::RegisterOK;
        }
        else
        {
            m_mysqlConn->rollback();
            responseMsg.resCode = ResponseCode::Failed;
            responseMsg.data1 = "用户数据插入失败...";
        }

    }

}

GameCommunication::GameCommunication() {
    // 加载mysql服务器配置
    Debug("加载mysql服务器配置.....");
    // 使用jsoncpp加载配置出现问题，暂不使用
//    ParseDBJson parse;
//    shared_ptr<DBInfo> info = parse.getDataBaseInfo(ParseDBJson::DBType::Mysql);

    // 连接mysql服务器
    m_mysqlConn = new MySqlConn;
//    bool flag = m_mysqlConn->connect(info->user, info->password, info->dbname, info->ip, info->port);
    bool flag = m_mysqlConn->connect("root", "123456", "ddz", "localhost");
    assert(flag);

    // 连接redis服务器
    m_redisConn = new RedisConn;
    assert(flag);
}

void GameCommunication::handleJoinRoom(std::shared_ptr<Message> requestMsg, Message &responseMsg) {



}

void GameCommunication::saveRsaKey(std::string field, std::string value) {
    m_redisConn->saveRsaKey(field, value);
}

std::string GameCommunication::getRsaKey(std::string field) {
    return m_redisConn->getRsaKey(field);
}