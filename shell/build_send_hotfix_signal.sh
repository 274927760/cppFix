#!/bin/bash

# 编译 C++ 源文件，并指定头文件目录
g++ -std=c++11 -I../inc -o ../bin/send_hotfix_signal ../src/send_hotfix_signal.cpp

echo "Compilation send_hotfix_signal complete, send_hotfix_signal build in ../bin/send_hotfix_signal"
