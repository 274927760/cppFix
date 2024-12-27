//
// Created by kwai on 2024/12/18.
//

#ifndef CPPFIX_SYMBOL_MANAGER_H
#define CPPFIX_SYMBOL_MANAGER_H

#include <iostream>
#include <unistd.h>   // For readlink()
#include <limits.h>   // For PATH_MAX
#include <elfio/elfio.hpp>
#include <unordered_map>
#include <pmap.h>
/*
主要用于做一些符号的查询，和一些地址的查询
*/
class Symbol_manager {
public:
    Symbol_manager();
    ~Symbol_manager();
    std::string get_cur_elf_path();
    void pre_deal_some_section();
    std::string get_elf_path();
    void open_so_and_load_symbols(const std::string &so_path);
    ELFIO::Elf64_Addr get_addr_by_name(const std::string &symbol_name);
    ELFIO::Elf64_Addr get_addr_from_some_so(const std::string &so_path, const std::string &symbol_name);
    void test_addr_terminal();

private:
    void init();
    std::string elf_path;// 当前进程所在的elf的路径
    ELFIO::elfio elf_reader; // 加载elf文件的对象
    Process_map_manager process_map_manager; // 进程管理器
    ELFIO::symbol_section_accessor *symbols_accessor = nullptr; // 创建符号表的访问器
    std::unordered_map<std::string, ELFIO::Elf64_Addr> func_table; // 函数符号对应的偏移地址 (代码段)
    std::vector<std::string> loaded_so_name; // 已经加载进来的so
    std::unordered_map<std::string, void*> so_handler_table; // 已经加载进来的所有so 的handler
//    std::unordered_map<std::string, ELFIO::Elf64_Addr> data_table; // 全局变量以及静态变量对应的偏移地址 (数据段)
};


#endif //CPPFIX_SYMBOL_MANAGER_H
