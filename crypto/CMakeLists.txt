cmake_minimum_required(VERSION 3.5)
project(crypto)
set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/ssl/include/")

# 搜索源文件和头文件
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(crypto STATIC ${SRC} ${HEADERS})
