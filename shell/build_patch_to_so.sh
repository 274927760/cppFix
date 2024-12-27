#!/bin/bash

# 检查输入参数
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <source_file_name_without_extension>"
    exit 1
fi

# 定义变量
SOURCE_FILE_NAME=$1
SOURCE_FILE_PATH="../patch/code/${SOURCE_FILE_NAME}.cpp"
OUTPUT_FILE_PATH="../patch/bin/${SOURCE_FILE_NAME}.so"

# 编译 C++ 源文件，并指定头文件目录
g++ -std=c++17 -shared -fPIC -o "$OUTPUT_FILE_PATH" "$SOURCE_FILE_PATH"

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "Compilation complete: ${SOURCE_FILE_PATH} -> ${OUTPUT_FILE_PATH}"
else
    echo "Compilation failed."
fi
