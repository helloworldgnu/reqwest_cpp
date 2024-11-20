cmake_minimum_required(VERSION 3.20)

project(crab_http_cpp)
set(CMAKE_CXX_STANDARD 17)

# 根据构建类型添加后缀
set(CMAKE_DEBUG_POSTFIX "d")

set(SOURCES
        client.cpp
        client_builder.cpp
        header_map.cpp
        http_exception.cpp
        proxy.cpp
        r_string.cpp
        request.cpp
        request_builder.cpp
        resp_body.cpp
        response.cpp
)

set(HEADERS
        client.h
        client_builder.h
        crab_http.h
        crab_http_c.h
        header_map.h
        http_exception.h
        proxy.h
        r_string.h
        request.h
        request_builder.h
        resp_body.h
        response.h
)

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

add_library(crab_http_cpp STATIC ${SOURCES} ${HEADERS})
add_library(crab_http_cpp::crab_http_cpp ALIAS crab_http_cpp)

target_include_directories(crab_http_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
