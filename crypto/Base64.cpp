#include "Base64.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/buffer.h"
#include <assert.h>

string Base64::enCode(string data)
{
    return enCode(data.data(), data.size());
}

string Base64::enCode(const char *data, int length)
{
    BIO *bio_b64 = BIO_new(BIO_f_base64());
    BIO *bio_mem = BIO_new(BIO_s_mem());
    assert(bio_b64 && bio_mem);

    // 链接BIO节点
    BIO_push(bio_b64, bio_mem);

    // 将数据写入base64节点
    BIO_write(bio_b64, data, length);
    BIO_flush(bio_b64);

    // 将内存中的数据写入 BUF_MEM 对象中
    BUF_MEM *p;
    BIO_get_mem_ptr(bio_b64, &p);

    string retData(p->data, p->length);

    // 释放资源
    BIO_free_all(bio_b64);

    return retData;
}

string Base64::deCode(string data)
{
    return deCode(data.data(), data.size());
}

string Base64::deCode(const char *data, int length)
{
    BIO *bio_b64 = BIO_new(BIO_f_base64());
    BIO *bio_mem = BIO_new(BIO_s_mem());
    assert(bio_b64 && bio_mem);

    // 链接BIO节点
    BIO_push(bio_b64, bio_mem);

    // 将数据写入内存节点
    BIO_write(bio_mem, data, length);

    char* out = new char[length];
    // 读取BIO节点中的数据
    int realLen = BIO_read(bio_b64, out, length);
    assert(realLen);

    string retData(out, realLen);

    // 释放资源
    delete[] out;
    BIO_free_all(bio_b64);

    return retData;
}

