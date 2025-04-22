cmake_minimum_required(VERSION 3.24)
project(DDZ_Server)
set(CMAKE_CXX_STANDARD 17)

# 添加头文件路径（更推荐用 target_include_directories）
include_directories(
        ${PROJECT_SOURCE_DIR}/common
        ${PROJECT_SOURCE_DIR}/http
        ${PROJECT_SOURCE_DIR}/reactor
        ${PROJECT_SOURCE_DIR}/tcp
        ${PROJECT_SOURCE_DIR}/thread
        ${PROJECT_SOURCE_DIR}/serialize
        ${PROJECT_SOURCE_DIR}/crypto
        ${PROJECT_SOURCE_DIR}/game
        ${PROJECT_SOURCE_DIR}/database
        # 包含openssl头文件
        "/usr/local/ssl/include/"
        # 包含protobuf jsoncpp头文件
        "/usr/local/include/"
        # 包含mysql头文件
        "/usr/include/mysql/"

        # 测试用例
        ${PROJECT_SOURCE_DIR}/test
)

# 添加子目录（这些子目录必须生成库）
add_subdirectory(common)
add_subdirectory(http)
add_subdirectory(reactor)
add_subdirectory(tcp)
add_subdirectory(thread)
add_subdirectory(serialize)
add_subdirectory(crypto)
add_subdirectory(game)
add_subdirectory(database)

# 添加链接的库文件目录
link_directories(
        "/usr/lib64/mysql/"
        "/usr/local/lib64/"
)

# 测试目录
add_subdirectory(test)

# 指定可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 创建可执行文件
add_executable(DDZ_Server main.cpp)

# 链接所有子目录生成的库
target_link_libraries(DDZ_Server
        PRIVATE
        tcp     # 链接 tcp 库
        http
        thread
        reactor
        common
        serialize
        crypto
        game
        database
        "/usr/local/ssl/lib64/libcrypto.so"

        test
)