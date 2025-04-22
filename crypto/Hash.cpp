//
// Created by thehao on 2025/4/13.
//

#include "Hash.h"
#include <assert.h>

Hash::Hash(HashType hashType, Hash::StoreType storeType) {
    m_hashType = hashType;
    m_storeType = storeType;

    // 创建并初始化hash函数上下文
    m_ctx = EVP_MD_CTX_new();
    assert(m_ctx);

    // 初始化指定hash函数的上下文
    int ret = EVP_DigestInit_ex(m_ctx, HashFuncMap.at(hashType)(), NULL);
    assert(ret);
}

Hash::~Hash() {
    if (m_ctx != NULL)
    {
        EVP_MD_CTX_free(m_ctx);
    }
}

void Hash::addData(string data) {
    addData(data.data(), data.size());
}

void Hash::addData(const char *data, int length) {
    // 更新hash函数上下文，处理计算摘要的数据
    int ret = EVP_DigestUpdate(m_ctx, data, length);
    assert(ret);
}

string Hash::result() {

    unsigned char md[m_hashDigestLenMap.at(m_hashType)];
    unsigned int len = 0;
    // 计算摘要
    // md:存储摘要结果的缓冲区指针
    // s:存储摘要结果长度的指针
    int ret = EVP_DigestFinal_ex(m_ctx, md, &len);
    assert(ret);

    string retData;
    if (m_storeType == StoreType::Binary)
    {
        retData = std::move(string(reinterpret_cast<char *>(md), len));
    }
    else
    {
        char hexData[len*2];
        for (int i = 0; i < len; ++i) {
            sprintf(&hexData[i*2], "%02x", md[i]);
        }
        retData = std::move(string(hexData, len*2));
    }
    return retData;
}


