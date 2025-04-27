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
#include "Room.h"
#include "Card.h"

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

    // 设置返回数据的函数
    sendMsgCallback sendMsg = m_sendMsgCallback;

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
        case RequestCode::AutoRoom:
            Debug("开始加入随机房间.........");
            handleJoinRoom(requestMsg, responseMsg);
            // 向房间中的客户端发送 人数/开始游戏 的信息
            // 设计 readyForPlay 的目的：
            // m_sendMsgCallback 同一时间只针对一个客户端发送信息(因为它被TcpConnection封装(发送函数与一个文件描述符))
            // 因此想要完成对多个客户端的数据发送，需要设计一个与m_sendMsgCallback类型相同的可调用对象
            // 在上述思想上增加对游戏是否开始的判断，最终函数就是 readyForPlay
            sendMsg = std::bind(&GameCommunication::readyForPlay, this, responseMsg.roomname, std::placeholders::_1);
            break;
        case RequestCode::JoinRoom:
            Debug("开始加入指定房间.........");
            handleJoinRoom(requestMsg, responseMsg);
            sendMsg = std::bind(&GameCommunication::readyForPlay, this, responseMsg.roomname, std::placeholders::_1);
            break;
        default:
            break;
    }

    // 对返回数据进行序列化编码
    Codec codec;
    string encodedMsg = codec.encodeMsg(&responseMsg);
    // 返回响应数据
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

    Room room;

    // 获取用户分数
    int score = 0;
    // 获取当前用户上一次游戏的房间
    std::string oldRoomname = room.currentRoom(requestMsg->username);
    if (oldRoomname != std::string())
    {
        // 不是第一次参加游戏，则从redis获取最新分数
        score = room.getPlayerScore(oldRoomname, requestMsg->username);
    }
    else
    {
        // 如果是首次加入房间，则从mysql数据库读取分数
        std::string sql = "select score from `information` where name = '" + requestMsg->username + "';";
        bool flag = m_mysqlConn->query(sql);
        assert(flag);
        // 获取一行查询到的数据
        m_mysqlConn->next();
        score = stoi(m_mysqlConn->value(0));
    }

    // 获取用户移入的房间
    std::string roomname;
    bool flag = true;
    if (requestMsg->reqCode == RequestCode::AutoRoom)
    {
        // 随机加入房间
        roomname = room.joinRoom(requestMsg->username);

    } else if (requestMsg->reqCode == RequestCode::JoinRoom)
    {
        // 加入指定房间
        roomname = requestMsg->roomname;
        flag = room.joinRoom(requestMsg->username, requestMsg->roomname);
    }

    if (!flag)
    {
        // 房间加入失败则直接退出
        responseMsg.resCode = ResponseCode::Failed;
        responseMsg.data1 = "房间已满，加入指定房间失败....";
        return;
    }

    // 更新 新房间中用户的分数
    room.updatePlayerScore(requestMsg->roomname, requestMsg->username, score);

    // 存储 <房间 - 用户 - 通信函数> 信息
    RoomList* roomList = RoomList::getInstance();
    Debug("未执行addInfo");
    roomList->addInfo(roomname, requestMsg->username, m_sendMsgCallback);
    Debug("已经执行addInfo");

    // 组织回复数据
    responseMsg.resCode = ResponseCode::JoinRoomOK;
    responseMsg.roomname = roomname;
    // 加入房间的人数
    requestMsg->data1 = std::to_string(room.playerCount(roomname));

    Debug("加入的房间为：%s, 人数为：%s", roomname.c_str(), requestMsg->data1.c_str());
}

void GameCommunication::saveRsaKey(std::string field, std::string value) {
    m_redisConn->saveRsaKey(field, value);
}

std::string GameCommunication::getRsaKey(std::string field) {
    return m_redisConn->getRsaKey(field);
}

void GameCommunication::readyForPlay(std::string roomname, std::string data) {

    Debug("readyForPlay.......");
    RoomList* roomList = RoomList::getInstance();
    UsersSendFuncMap players = roomList->getUsersSendFuncMap(roomname);

    // 将序列化后的response信息发送给房间内的每个用户
    for (auto pair: players)
    {
        // 通知各个客户端
        pair.second(data);
        Debug("当前房间内人数: %d==================", players.size());
    }
    // 如果房间人数足够，则继续发送游戏数据
    if (players.size() == 3)
    {
        startGame(roomname, players);
    }

}

void GameCommunication::startGame(std::string roomname, UsersSendFuncMap players) {
    // 生成卡牌信息
    Card card;
    // 初始化牌组
    card.initCards();
    // 获取手牌信息
    std::string handCards = card.getHandCards();
    // 获取底牌信息
    std::string lastCards = card.getLastCards();
    // 获取用户出牌顺序
    std::string order = getPlayersOrder(roomname);

    // 组织回复的数据
    Message msg;
    msg.resCode = ResponseCode::StartGame;
    msg.roomname = roomname;
    msg.data1 = handCards;
    msg.data2 = lastCards;
    msg.data3 = order;

    Codec codec;
    std::string data = codec.encodeMsg(&msg);

    // 将数据发送个客户端们
    for (auto player : players)
    {
        player.second(data);
    }

}

std::string GameCommunication::getPlayersOrder(std::string roomname) {

    auto redis = m_redisConn->getRedis();

    // 排序后的结果集
    std::vector<std::pair<std::string, double>> result;
    // score降序取出每个用户的信息
    redis->zrevrange(roomname, 0, -1, back_inserter(result));

    // 组织次序数据:   用户名-次序-分数#
    int order = 1;
    std::string orderStr;
    for (const auto userInfo : result)
    {
        std::string data = userInfo.first + '-' + to_string(order++) + '-' + to_string((int)userInfo.second) + '#';
        orderStr += data;
    }

    return orderStr;
}
