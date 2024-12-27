//
// Created by kwai on 2024/12/18.
//

#ifndef CPPFIX_HOTFIX_H
#define CPPFIX_HOTFIX_H


#include <unordered_map>
#include <iostream>
#include <elfio/elfio.hpp>
extern unsigned char jump_code[12];
class Hotfix{
public:
    Hotfix(const std::string& hotfix_name, ELFIO::Elf64_Addr old_func_addr,
           ELFIO::Elf64_Addr new_func_addr, const std::string &old_func_symbol_name,
           const std::string &new_func_symbol_name):
           _hotfix_name(hotfix_name), _old_func_addr(old_func_addr), _new_func_addr(new_func_addr),
           _old_func_symbol_name(old_func_symbol_name), _new_func_symbol_name(new_func_symbol_name){
        std::cout << hotfix_name << " create suc!!!" <<std::endl;
        print_info();
    }
    void hook();
    void unHook();
    void print_info();
private:
    std::string _hotfix_name;
    ELFIO::Elf64_Addr _old_func_addr;  // 老函数的地址
    ELFIO::Elf64_Addr _new_func_addr; // 新函数的地址
    std::string _old_func_symbol_name; // 旧函数的符号
    std::string _new_func_symbol_name; // 新函数的符号
    unsigned char _old_func_addr_original_code[sizeof(jump_code)]{}; // 原始的指令内容
};

class Hotfix_manager{
public:
    Hotfix_manager()=default;
    ~Hotfix_manager(){
        for(auto&[name, hotfix_p]: hotfix_table){
            delete hotfix_p;
        }
        hotfix_table.clear();
    }
    void add_hotfix_and_execute(const std::string& hotfix_name, const std::string &patch_so_name, const std::string &old_func_symbol_name, const std::string& new_func_symbol_name);
    void recall_some_hotfix(const std::string &hotfix_name); // 撤回某个指定的hotfix
private:
    std::unordered_map<std::string, Hotfix*> hotfix_table; // 记录了已经被 hotfix的函数 信息 方便做还原
};

#endif //CPPFIX_HOTFIX_H
