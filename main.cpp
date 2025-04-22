#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "TcpServer.h"

#include "MyTest.h"
#include "AesCrypto.h"
#include "RsaCrypto.h"
#include <string>

void testAesCrypto()
{
    AesCrypto aes(AesCrypto::AesHashAlgorithm::AES_CBC_128, "1234567887654321");
    std::string text("我们在调用MySQL API的使用需要加载的动态库为libmysql.dll，它对应的导入库为libmysql.lib，在该窗口的附加依赖项位置指定的就是这个导入库的名字。编译编写好的项目之后，在对应的项目目录中会生成一个可执行程序，打开这个目录，将上面步骤中下载的用于MySQL数据库加密的动态库拷贝到该目录中，这样程序就可以正常执行了，否则会提示无法加载某个动态库。");
    std::string enCrypyoText = aes.enCrypto(text);
    std::cout << "加密数据：" << enCrypyoText << std::endl;

    std::string deCryptoText = aes.deCrypto(enCrypyoText);
    std::cout << "解密数据：" << deCryptoText.data() << std::endl;
}

void testRsaCrypto()
{
    RsaCrypto rsa;
    rsa.generatePkey(RsaCrypto::BITS_2k);
    std::cout << "正在生成密钥对..." << std::endl;

    std::string text("我们在调用MySQL");

    RsaCrypto rsa1("public.pem", RsaCrypto::PublicKey);
    std::string enCrypyoText = rsa1.enCrypto(text);
    std::cout << "加密数据：" << enCrypyoText << std::endl;

    RsaCrypto rsa2("private.pem", RsaCrypto::PrivateKey);
    std::string deCryptoText = rsa2.deCrypto(enCrypyoText);
    std::cout << "解密数据：" << deCryptoText.data() << std::endl;

    std::string signedData = rsa2.sign(deCryptoText);
    bool flag = rsa.verify(signedData, text);
    std::cout << "校验结果：" << flag << std::endl;

}

int main(int argc, char* argv[])
{
#if 0
    if (argc < 3)
    {
        printf("./a.out port path\n");
        return -1;
    }
    unsigned short port = atoi(argv[1]);
    // 切换服务器的工作路径
    chdir(argv[2]);
#else
    unsigned short port = 10000;
//    chdir("/home");
#endif
    // 启动服务器
    TcpServer* server = new TcpServer(port, 4);
    server->run();

//    testAesCrypto();
//    testRsaCrypto();

//    MyTest my_test;
//    my_test.test();

    return 0;
}