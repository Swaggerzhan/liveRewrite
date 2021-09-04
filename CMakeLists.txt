cmake_minimum_required(VERSION 3.19)
project(liveRewrite)

set(CMAKE_CXX_STANDARD 14)

add_executable(liveRewrite main.cpp base/timeval.h base/timeval.cpp base/handlerset.h base/handlerset.cpp)