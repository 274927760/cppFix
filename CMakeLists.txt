cmake_minimum_required(VERSION 2.8.4)
project(cppFix)
add_compile_options(-std=c++17)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
# 开启地址随机化
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "-fPIE")
set(CMAKE_EXE_LINKER_FLAGS "-pie")
include_directories(${CMAKE_SOURCE_DIR}/3rd) #  第三方库
include_directories(${CMAKE_SOURCE_DIR}/inc)
add_executable(cppFix
        main.cpp
        src/symbol_manager.cpp
        src/pmap.cpp)