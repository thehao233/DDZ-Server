cmake_minimum_required(VERSION 3.5)
project(serialize)

#include_directories(
#    "D:/software/Protobuf/Protobuf/include"
#)

# 搜索源文件和头文件
aux_source_directory(./ SRC)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(serialize STATIC ${SRC} ${HEADERS})

#target_link_libraries(serialize PRIVATE Qt${QT_VERSION_MAJOR}::Core "D:/software/Protobuf/Protobuf/lib/libprotobuf.dll")
