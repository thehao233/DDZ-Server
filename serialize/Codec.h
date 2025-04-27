#ifndef CODEC_H
#define CODEC_H

#include "Information.pb.h"

#include <string>
#include <memory>
using namespace std;

struct Message
{
    string username;
    string roomname;
    string data1;
    string data2;
    string data3;
    RequestCode reqCode;
    ResponseCode resCode;
};

class Codec
{
public:
    Codec() = default;
    Codec(string codedMsg);
    Codec(Message* uncodedMsg);

    // 数据编解码
    string encodeMsg(Message* uncodedMsg);
    shared_ptr<Message> decodeMsg(string codedMsg);

    // 重新装载数据
    void reload(string codedMsg);
    void reload(Message* uncodedMsg);

private:
    string m_codedMsg;
    // 用于处理数据的编解码
    Information m_inf;
};

#endif // CODEC_H
