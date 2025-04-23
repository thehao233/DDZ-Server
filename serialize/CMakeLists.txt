cmake_minimum_required(VERSION 3.5)
project(serialize)
set(CMAKE_CXX_STANDARD 17)

# 搜索源文件和头文件
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(serialize STATIC ${SRC} ${HEADERS})