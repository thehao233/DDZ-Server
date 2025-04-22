//
// Created by thehao on 2025/3/28.
//

#include "Person.pb.h"
#include <iostream>
#include <string>

#ifndef DDZ_SERVER_MYTEST_H
#define DDZ_SERVER_MYTEST_H

class MyTest
{
public:
    void test();
};

void MyTest::test() {
    Person p;
    p.set_name("aaa");
    p.set_age(10);
    p.set_id(1);
    p.set_sex("男");

    std::string s;
    p.SerializeToString(&s);

    Person p2;
    p2.ParseFromString(s);
    std::cout <<  p2.name();
}

#endif //DDZ_SERVER_MYTEST_H
