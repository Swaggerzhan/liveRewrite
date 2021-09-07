cmake_minimum_required(VERSION 3.5.1)
project(liveRewrite)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty)

find_package(Threads REQUIRED)

file(GLOB SRC_NET_FILE "*.cc" "*.c" "3rdparty/muduo/net/*.cc")
file(GLOB SRC_BASE_FILE "*.cc" "*.c" "3rdparty/muduo/base/*.cc")

file(GLOB INC_NET_FILE "*.h" "*.hpp" "3rdparty/muduo/net/*.h")
file(GLOB INC_BASE_FILE "*.h" "*.hpp" "3rdparty/muduo/base/*.h")

set( ALL_FILE
        ${SRC_NET_FILE}
        ${SRC_BASE_FILE}
        ${INC_NET_FILE}
        ${INC_BASE_FILE}
        base/mediasession.cpp base/mediasession.h base/rtpconnection.h base/rtpconnection.cpp base/rtsp.h base/rtspserver.h base/rtspserver.cpp)

message(INFO ${ALL_FILE})

add_executable(liveRewrite main.cpp
        base/timeval.h base/timeval.cpp
        base/handlerset.h base/handlerset.cpp
        base/rtsprequest.h base/rtsprequest.cpp
        base/rtspconnection.h base/rtspconnection.cpp
        base/rtp.h base/media.h base/media.cpp ${ALL_FILE}
        # 添加poller
        3rdparty/muduo/net/poller/EPollPoller.h
        3rdparty/muduo/net/poller/EPollPoller.cc
        base/mediasession.h base/mediasession.cpp
        base/rtpconnection.h base/rtpconnection.cpp
        base/rtsp.h
        base/rtspserver.h base/rtspserver.cpp
        )

target_link_libraries(liveRewrite ${CMAKE_THREAD_LIBS_INIT})

# target_sources(liveRewrite ${SC_FILES})
#target_sources(liveRewrite ${PROJECT_SOURCE_DIR}/3rdparty/muduo/base/*.cc)
#target_sources(liveRewrite ${PROJECT_SOURCE_DIR}/3rdparty/muduo/net/*.h)
#target_sources(liveRewrite ${PROJECT_SOURCE_DIR}/3rdparty/muduo/net/*.cc)

