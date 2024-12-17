cmake_minimum_required(VERSION 2.8.4)
project(cppFix)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
# 开启地址随机化
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "-fPIE")
set(CMAKE_EXE_LINKER_FLAGS "-pie")

add_executable(cppFix
        main.cpp)
