#pragma once
#include <string>
#include <iostream>
using namespace std;

class Buffer
{
public:
    Buffer(int size);
    ~Buffer();

    // 扩容
    void extendRoom(int size);
    // 得到剩余的可写的内存容量
    inline int writeableSize()
    {
        return m_capacity - m_writePos;
    }
    // 得到剩余的可读的内存容量
    inline int readableSize()
    {
//        std::cout << "readPos:" << m_readPos << " writePos:" << m_writePos << std::endl;
        return m_writePos - m_readPos;
    }
    // 写内存 1. 直接写 2. 接收套接字数据
    int appendString(const char* data, int size);
    int appendString(const char* data);
    int appendString(const string data);

    int appendDataHead(int headLen);
    int appendDataPackage(const string data);

    int socketRead(int fd);
    // 根据\r\n取出一行, 找到其在数据块中的位置, 返回该位置
    char* findCRLF();
    // 发送数据
    int sendData(int socket);    // 指向内存的指针
    // 得到读数据的起始位置
    inline char* data()
    {
        return m_data + m_readPos;
    }
    // 根据参数读出指定数量的数据
    inline string data(int length)
    {
        string msg(m_data + m_readPos, length);
        m_readPos += length;
        return msg;
    }
    inline int readPosIncrease(int count)
    {
        m_readPos += count;
        return m_readPos;
    }
private:
    char* m_data;
    int m_capacity;
    int m_readPos = 0;
    int m_writePos = 0;
};

