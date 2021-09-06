cmake_minimum_required(VERSION 3.5.1)
project(liveRewrite)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty)



add_executable(liveRewrite main.cpp
        base/timeval.h base/timeval.cpp
        base/handlerset.h base/handlerset.cpp
        base/rtsprequest.h base/rtsprequest.cpp
        base/rtspconnection.h base/rtspconnection.cpp
        base/rtp.h base/media.h base/media.cpp)

target_sources(liveRewrite ${PROJECT_SOURCE_DIR}/3rdparty/muduo/base/*.h)
target_sources(liveRewrite ${PROJECT_SOURCE_DIR}/3rdparty/muduo/base/*.cc)
target_sources(liveRewrite ${PROJECT_SOURCE_DIR}/3rdparty/muduo/net/*.h)
target_sources(liveRewrite ${PROJECT_SOURCE_DIR}/3rdparty/muduo/net/*.cc)

