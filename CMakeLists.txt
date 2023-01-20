# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.20.2)

# 项目信息
project (multimedia VERSION 1.0)
configure_file(version.h.in version.h) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(THIRD_PARTY "${PROJECT_SOURCE_DIR}/third_party")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wall -fPIC -D_GNU_SOURCE")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb")#Debug模式下的编译指令
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")#Release模式下的编译指令



add_subdirectory(ffmpeg_sdk)
