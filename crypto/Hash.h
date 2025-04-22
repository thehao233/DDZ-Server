//
// Created by thehao on 2025/4/13.
//

#ifndef DDZ_SERVER_HASH_H
#define DDZ_SERVER_HASH_H

#include <string>
#include <map>
#include <openssl/evp.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
using namespace std;

enum class HashType:char {
    Md4,
    Md5,
    Sha1,
    Sha224,
    Sha256,
    Sha384,
    Sha512,
    Sha3_224,
    Sha3_256,
    Sha3_384,
    Sha3_512,
};

using HashFunc = const EVP_MD *(*)(void);
const map<HashType, HashFunc> HashFuncMap{
        {HashType::Md4, EVP_md4},
        {HashType::Md5, EVP_md5},
        {HashType::Sha1, EVP_sha1},
        {HashType::Sha224, EVP_sha224},
        {HashType::Sha256, EVP_sha256},
        {HashType::Sha384, EVP_sha384},
        {HashType::Sha512, EVP_sha512},
        {HashType::Sha3_224, EVP_sha3_224},
        {HashType::Sha3_256, EVP_sha3_256},
        {HashType::Sha3_384, EVP_sha3_384},
        {HashType::Sha3_512, EVP_sha3_512},
};

class Hash {

public:
    enum class StoreType:char{Binary, Hex};

    Hash(HashType hashType, StoreType storeType = StoreType::Binary);
    ~Hash();

    // 添加待计算的数据
    void addData(string data);
    void addData(const char* data, int length);

    // hash 计算的结果
    string result();

private:
    EVP_MD_CTX *m_ctx = NULL;
    HashType m_hashType;
    StoreType m_storeType;

    const map<HashType, int> m_hashDigestLenMap{
            {HashType::Md4, MD4_DIGEST_LENGTH},
            {HashType::Md5, MD5_DIGEST_LENGTH},
            {HashType::Sha1, SHA_DIGEST_LENGTH},
            {HashType::Sha224, SHA224_DIGEST_LENGTH},
            {HashType::Sha256, SHA256_DIGEST_LENGTH},
            {HashType::Sha384, SHA384_DIGEST_LENGTH},
            {HashType::Sha512, SHA512_DIGEST_LENGTH},
            {HashType::Sha3_224, SHA224_DIGEST_LENGTH},
            {HashType::Sha3_256, SHA256_DIGEST_LENGTH},
            {HashType::Sha3_384, SHA384_DIGEST_LENGTH},
            {HashType::Sha3_512, SHA512_DIGEST_LENGTH},
    };
};


#endif //DDZ_SERVER_HASH_H
