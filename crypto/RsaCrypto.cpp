#include "RsaCrypto.h"
#include "openssl/pem.h"
#include <assert.h>
#include <iostream>

RsaCrypto::RsaCrypto(string fileName, KeyType type)
{
    // 将存储在磁盘的密钥文件读取出来
    BIO *bio = BIO_new_file(fileName.data(), "rb");
    assert(bio);

    switch (type) {
        case PublicKey:
        {
            PEM_read_bio_PUBKEY(bio, &m_publicKey, NULL, NULL);
            break;
        }
        case PrivateKey:
        {
            PEM_read_bio_PrivateKey(bio, &m_privateKey, NULL, NULL);
            break;
        }
    }

    // 释放资源
    BIO_free(bio);
}

RsaCrypto::~RsaCrypto()
{
    if(m_publicKey != NULL)
    {
        EVP_PKEY_free(m_publicKey);
    }

    if(m_privateKey != NULL)
    {
        EVP_PKEY_free(m_privateKey);
    }
}

void RsaCrypto::generatePkey(KeyLength bits, string pub, string pri)
{
    // 生成秘钥上下文
    // id : 公钥算法的标识符，用于指定要使用的加密算法。可以使用EVP_PKEY_RSA、EVP_PKEY_EC常量
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    assert(ctx);

    // 初始化密钥生成操作的参数和上下文 成功则返回1
    int ret = EVP_PKEY_keygen_init(ctx);
    assert(ret);

    // 设置生成密钥对的位数
    ret = EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits);
    assert(ret);

    // 生成秘钥对
    // EVP_PKEY *pkey;
    // 直接使用上述申请出的pkey存在的问题：
    // 1. pkey不能在该方法结束时释放，这会导致程序崩溃
    // 2. pkey不能设置为类成员变量，这会导致verify函数中的 EVP_PKEY_CTX *ctx =EVP_PKEY_CTX_new(m_privateKey, NULL); ctx出现段错误
    ret = EVP_PKEY_generate(ctx, &m_privateKey);
    assert(ret);

    // 将秘钥对存储到磁盘
    // 存储私钥
    BIO *bio = BIO_new_file(pri.data(), "wb");
    assert(bio);
    // 将秘钥写入到.pem磁盘文件
    // `enc`：表示私钥加密算法的对称密码的 `EVP_CIPHER` 对象指针。如果不需要加密私钥，可以将其设为 `NULL`。
    // `kstr`：表示用于加密私钥的密钥的指针。
    // `klen`：表示用于加密私钥的密钥的长度。
    // `cb`：是一个回调函数，用于提供加密密钥的密码。当私钥加密使用了密码时，需要提供回调函数来输入密码，如果不需要密码，则可以将其设为 `NULL`。
    // `u`：是传递给回调函数的用户数据指针。
    ret = PEM_write_bio_PrivateKey(bio, m_privateKey, NULL, NULL, 0, NULL, NULL);
    assert(ret);
    BIO_flush(bio);
    BIO_free(bio);

    // 存储公钥
    bio = BIO_new_file(pub.data(), "wb");
    ret = PEM_write_bio_PUBKEY(bio, m_privateKey);
    assert(ret);
    BIO_flush(bio);
    BIO_free(bio);

    // 释放资源
    // EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
}

void RsaCrypto::parseStringToKey(string data, KeyType type)
{
    // 将存储在内存中的密钥文件读取出来
    BIO *bio = BIO_new_mem_buf(data.data(), data.size());
    assert(bio);

    switch (type) {
        case PublicKey:
        {
            PEM_read_bio_PUBKEY(bio, &m_publicKey, NULL, NULL);
            break;
        }
        case PrivateKey:
        {
            PEM_read_bio_PrivateKey(bio, &m_privateKey, NULL, NULL);
            break;
        }
    }

    // 释放资源
    BIO_free(bio);
}

string RsaCrypto::enCrypto(string data)
{
    // 生成秘钥上下文
    // `pkey`：与上下文关联的密钥对象。可以是一个公钥、私钥或对称密钥对象，具体取决于使用场景。
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(m_publicKey, NULL);
    assert(ctx);

    // 初始化加密操作的函数
    int ret = EVP_PKEY_encrypt_init(ctx);
    assert(ret);

    // 对数据进行填充
    /*
        `pad`：要设置的加密填充（padding）方式，可以是以下值之一：

        - `RSA_PKCS1_PADDING`：PKCS#1 填充方式，是最常见的 RSA 填充方式。
        - `RSA_PKCS1_OAEP_PADDING`：PKCS#1 OAEP 填充方式，带有随机性质的填充方式，安全性更高。
        - `RSA_NO_PADDING`：不进行填充操作，仅加密或解密数据。
    */
    ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
    assert(ret);

    // 开始加密
    size_t outlen;
    // 用于获取加密后数据长度
    ret = EVP_PKEY_encrypt(ctx, NULL, &outlen,
                           reinterpret_cast<const unsigned char *>(data.data()), data.size());
    assert(ret);

    // 加密操作
    unsigned char *out = new unsigned char[outlen];
    ret = EVP_PKEY_encrypt(ctx, out, &outlen,
                           reinterpret_cast<const unsigned char*>(data.data()), data.size());
    assert(ret);

    // 对加密后的数据进行Base64编码
    string retData = m_base64.enCode(reinterpret_cast<char*>(out), outlen);

    // 释放资源
    delete[] out;
    EVP_PKEY_CTX_free(ctx);
    return retData;
}

string RsaCrypto::deCrypto(string data)
{
    // 先进行Base64解码
    data = m_base64.deCode(data);

    // 生成秘钥上下文
    // `pkey`：与上下文关联的密钥对象。可以是一个公钥、私钥或对称密钥对象，具体取决于使用场景。
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(m_privateKey, NULL);
    assert(ctx);

    // 初始化解密操作的函数
    int ret = EVP_PKEY_decrypt_init(ctx);
    assert(ret);

    // 对数据进行填充
    /*
        `pad`：要设置的加密填充（padding）方式，可以是以下值之一：

        - `RSA_PKCS1_PADDING`：PKCS#1 填充方式，是最常见的 RSA 填充方式。
        - `RSA_PKCS1_OAEP_PADDING`：PKCS#1 OAEP 填充方式，带有随机性质的填充方式，安全性更高。
        - `RSA_NO_PADDING`：不进行填充操作，仅加密或解密数据。
    */
    ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
    assert(ret);

    // 开始解密
    size_t outlen;
    // 用于获取解密后数据长度
    ret = EVP_PKEY_decrypt(ctx, NULL, &outlen,
                           reinterpret_cast<const unsigned char*>(data.data()), data.size());
    assert(ret);

    // 解密操作
    unsigned char *out = new unsigned char[outlen];
    ret = EVP_PKEY_decrypt(ctx, out, &outlen,
                           reinterpret_cast<const unsigned char*>(data.data()), data.size());
    assert(ret);

    string retData(reinterpret_cast<char*>(out), outlen);

    // 释放资源
    delete[] out;
    EVP_PKEY_CTX_free(ctx);
    return retData;
}

string RsaCrypto::sign(string data, HashType hashAlgo)
{
    // 对hash计算后的数据进行签名

    // hash计算
    Hash hs(hashAlgo);
    hs.addData(data);
    string hashedData = hs.result();

    // 生成秘钥上下文
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(m_privateKey, NULL);
    assert(ctx);

    // 初始化签名操作函数
    int ret = EVP_PKEY_sign_init(ctx);
    assert(ret);

    // 设置签名算法的函数
    // 该函数应当与进行hash计算的函数名相对应
    ret = EVP_PKEY_CTX_set_signature_md(ctx, HashFuncMap.at(hashAlgo)());
    assert(ret);

    // 使用非对称加密秘钥进行签名
    size_t siglen;
    // 获取签名后数据的长度
    ret = EVP_PKEY_sign(ctx, NULL, &siglen,
                        reinterpret_cast<const unsigned char *>(hashedData.data()), hashedData.size());
    assert(ret);

    unsigned char *sig = new unsigned char[siglen];
    // 签名操作
    ret = EVP_PKEY_sign(ctx, sig, &siglen,
                        reinterpret_cast<const unsigned char *>(hashedData.data()), hashedData.size());

    // 对签名后的数据进行Base64编码
    string signData =m_base64.enCode(reinterpret_cast<char*>(sig), siglen);
    assert(ret);

    // 释放资源
    delete[] sig;
    EVP_PKEY_CTX_free(ctx);
    return signData;
}

bool RsaCrypto::verify(string sign, string data, HashType hashAlgo)
{
    // 先对签名后的数据进行Base64解码
    sign = m_base64.deCode(sign);

    // 对hash计算后的数据进行签名

    // hash计算
    Hash hs(hashAlgo);
    hs.addData(data);
    string hashedData = hs.result();

    // 生成秘钥上下文
    EVP_PKEY_CTX *ctx =EVP_PKEY_CTX_new(m_privateKey, NULL);
    assert(ctx);

    // 初始化签名操作函数
    int ret = EVP_PKEY_verify_init(ctx);
    assert(ret);

    // 设置签名算法的函数
    // 该函数应当与进行hash计算的函数名相对应
    ret = EVP_PKEY_CTX_set_signature_md(ctx, HashFuncMap.at(hashAlgo)());
    assert(ret);

    // 进行签名校验
    ret = EVP_PKEY_verify(ctx,
                        reinterpret_cast<const unsigned char *>(sign.data()), sign.size(),
                        reinterpret_cast<const unsigned char *>(hashedData.data()), hashedData.size());

    // 释放资源
    EVP_PKEY_CTX_free(ctx);

    switch (ret) {
        case -1:
        {
            cout << "验签操作出现错误..." << endl;
            break;
        }
        case 0:
        {
            cout << "验签失败..." << endl;
            break;
        }
        case 1:
        {
            cout << "验签成功~~~" << endl;
            return true;
        }
    }

    return false;
}
