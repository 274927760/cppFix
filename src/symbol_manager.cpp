//
// Created by kwai on 2024/12/18.
//

#include "symbol_manager.h"
#include "utils.h"
#include <regex>
#include <pmap.h>
#include <elfio/elfio.hpp>

Symbol_manager::Symbol_manager() {
    elf_path = get_cur_elf_path();  // 存下当前elf文件的路径
    process_map_manager = Process_map_manager(elf_path);
    init();
}

// 这里为一些初始化的逻辑
// 读取elf
// 把符号表预处理进来，方便做查询
void Symbol_manager::init()
{
    // 读取 ELF 文件
    if (!elf_reader.load(elf_path)) {
        std::cerr << "Failed to load ELF file." << std::endl;
        return;
    }
    // 获取 ELF 文件头信息
    std::cout << "ELF Header Information:" << std::endl;
    // 输出文件类型
    std::cout << "File type: ";
    //todo 这里应该根据文件类型和aslr进行一些决策，是否需要读取段的基地址
    switch (elf_reader.get_type()) {
        case ELFIO::ET_EXEC:
            std::cout << "Executable" << std::endl;
            break;
        case ELFIO::ET_DYN:
            std::cout << "Shared object" << std::endl;
            break;
        case ELFIO::ET_REL:
            std::cout << "Relocatable" << std::endl;
            break;
        default:
            std::cout << "Unknown" << std::endl;
    }
    for (size_t i = 0; i < elf_reader.sections.size(); ++i) {
        ELFIO::section* sec = elf_reader.sections[i];
        if(sec -> get_type() == ELFIO::SHT_SYMTAB)
        {
            symbols_accessor = new ELFIO::symbol_section_accessor(elf_reader, sec);
//            break;
        }
    }
    if (symbols_accessor == nullptr) {
        std::cerr << "No symbol table found in ELF file." << std::endl;
        return;
    }
    pre_deal_some_section();
}



// 预处理一些段 如数据段和代码段
void Symbol_manager::pre_deal_some_section(){
    // 获取符号的数量
    size_t num_symbols = symbols_accessor->get_symbols_num();
    std::cout << "Number of symbols: " << num_symbols << std::endl;
    // 遍历所有符号
    for (size_t i = 0; i < num_symbols; ++i) {
        // 获取符号的详细信息
        std::string name;
        ELFIO::Elf64_Addr value;
        ELFIO::Elf_Xword size;
        unsigned char bind, type, other;
        ELFIO::Elf_Half section_index;

        // 获取符号的各项信息
        if (symbols_accessor->get_symbol(i, name, value, size, bind, type, section_index, other)) {
            // 使用 std::regex_search 来查找
            if (type == ELFIO::STT_FUNC) {
                func_table[name] = value + process_map_manager.get_code_start_addr();
//                std::cout << "Symbol " << i << ": " << std::endl;
//                std::cout << "  Name: " << name << std::endl;
////            printf(" Value: %p\n", value);
//                std::cout << "  Value: " << std::hex << value << std::dec << std::endl;
//                std::cout << "  Size: " << size << std::endl;
//                std::cout << "  Bind: " << (int)bind << std::endl;
//                std::cout << "  Type: " << (int)type << std::endl;
//                std::cout << "  Section Index: " << section_index << std::endl;
//                std::cout << "  Other: " << (int)other << std::endl;
//                std::cout << "---------------------------------" << std::endl;
            }
        }
    }
    std::cout << func_table.size() << std::endl;
}


ELFIO::Elf64_Addr Symbol_manager::get_addr_by_name(const std::string &symbol_name)
{
    ELFIO::Elf64_Addr res = 0;
    if(symbol_name.empty()) return res;
    // 遍历所有符号
    for (size_t i = 0; i < symbols_accessor->get_symbols_num(); ++i) {
        // 获取符号的详细信息
        std::string name;
        ELFIO::Elf64_Addr value;
        ELFIO::Elf_Xword size;
        unsigned char bind, type, other;
        ELFIO::Elf_Half section_index;

        // 获取符号的各项信息
        if (symbols_accessor->get_symbol(i, name, value, size, bind, type, section_index, other)
            and symbol_name == name) {
                res = type == ELFIO::STT_FUNC ? process_map_manager.get_code_start_addr() + value
                                                :
                                                process_map_manager.get_data_start_addr() + value;
                break;
        }
    }
    return res;
}

void Symbol_manager::test_addr_terminal(){
    while(true)
    {
        std::cout << "please input a symbol name : ";
        std::string symbol_name;
        std::cin >> symbol_name;
        ELFIO::Elf64_Addr addr = get_addr_by_name(symbol_name);
        if(addr == 0) // 说明符号不存在与代码段
        {
            std::cout << "cur symbol does not in func_table !!!" << std::endl;
            continue;
        }
        std::cout << "cur addr = " << std::hex << func_table[symbol_name] << std::dec << std::endl;
    }
}

std::string Symbol_manager::get_cur_elf_path(){
    std::string res = "";
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0'; // Null-terminate the path
        std::cout << "Executable path: " << path << std::endl;
        res = std::string(path);
    } else {
        perror("readlink"); // Print error message if readlink fails
    }
    return res;
}

std::string Symbol_manager::get_elf_path(){
    return elf_path;
}

Symbol_manager::~Symbol_manager() {
    if(symbols_accessor != nullptr)
    {
        delete symbols_accessor;
        symbols_accessor = nullptr;
    }
}