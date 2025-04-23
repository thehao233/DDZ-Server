cmake_minimum_required(VERSION 3.24)
project(common)
set(CMAKE_CXX_STANDARD 17)

# 取出当前目录下的所有源文件，保存到变量SRC
aux_source_directory(./ SRC)
# 将源文件制作为静态库
add_library(common STATIC ${SRC})

target_link_libraries(common jsoncpp redis++)