#ifndef BASE64_H
#define BASE64_H

#include "string"

using namespace std;
class Base64
{
public:
    Base64() = default;

    // 编码
    string enCode(string data);
    string enCode(const char* data, int length);


    // 解码
    string deCode(string data);
    string deCode(const char* data, int length);
};

#endif // BASE64_H
