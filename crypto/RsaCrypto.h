#ifndef RSACRYPTO_H
#define RSACRYPTO_H

#include <map>
#include "openssl/evp.h"
#include "Base64.h"
#include <string>
#include "Hash.h"

using namespace std;
class RsaCrypto
{
public:

    enum KeyType{PublicKey, PrivateKey};

    explicit RsaCrypto() = default;
    // 构造并读取磁盘中的秘钥数据
    RsaCrypto(string fileName, KeyType type);
    ~RsaCrypto();

    // 密钥对生成的位数
    enum KeyLength{
        BITS_1k = 1024,
        BITS_2k = 2048,
        BITS_3k = 3072,
        BITS_4k = 4096,
    };


    // 生成密钥对
    // 指定秘钥对存放的文件位置
    void generatePkey(KeyLength bits, string pub = "public.pem", string pri = "private.pem");

    // 从内存中读取秘钥
    void parseStringToKey(string data, KeyType type);
    void readKey(string fileName, KeyType type);

    // 使用公钥进行加密
    string enCrypto(string data);

    // 使用私钥进行解密
    string deCrypto(string data);

    // 指定hash函数，并对计算后的数据进行数字签名
    string sign(string data, HashType hashAlgo = HashType::Sha256);

    // 进行校验
    bool verify(string sign, string data, HashType hashAlgo = HashType::Sha256);

private:
    EVP_PKEY *m_publicKey = NULL;
    EVP_PKEY *m_privateKey = NULL;
    // 用于生成密钥对，由于使用完成时间未定，因此在对象析构时释放
    EVP_PKEY *m_pkey = NULL;

    // 用于对二进制数据进行Base64编/解码
    Base64 m_base64;
};

#endif // RSACRYPTO_H
