#include "Codec.h"

Codec::Codec(string codedMsg)
{
    reload(codedMsg);
}

Codec::Codec(Message *uncodedMsg)
{
    reload(uncodedMsg);
}

string Codec::encodeMsg(Message *uncodedMsg)
{
    // 设置编解码器数据
    m_inf.set_username(uncodedMsg->username);
    m_inf.set_data1(uncodedMsg->data1);
    m_inf.set_data2(uncodedMsg->data2);
    m_inf.set_data3(uncodedMsg->data3);
    m_inf.set_reqcode(uncodedMsg->reqCode);
    m_inf.set_rescode(uncodedMsg->resCode);
    std::string out;
    m_inf.SerializeToString(&out);
    // 设置编码后的数据
    m_codedMsg = std::move(out);
    return m_codedMsg;
}

shared_ptr<Message> Codec::decodeMsg(string codedMsg)
{
    // 设置编码后的数据
    m_codedMsg = std::move(codedMsg);
    // 设置编解码器数据
    m_inf.ParseFromString(m_codedMsg);
    // 设置编码后的数据
    shared_ptr<Message> uncodeMsg(new Message);
    uncodeMsg->username = m_inf.username();
    uncodeMsg->data1 = m_inf.data1();
    uncodeMsg->data2 = m_inf.data2();
    uncodeMsg->data3 = m_inf.data3();
    uncodeMsg->reqCode = m_inf.reqcode();
    uncodeMsg->resCode = m_inf.rescode();
    return uncodeMsg;
}

void Codec::reload(string codedMsg)
{
    decodeMsg(codedMsg);
}

void Codec::reload(Message *uncodedMsg)
{
    encodeMsg(uncodedMsg);
}
