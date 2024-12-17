//
// Created by kwai on 2024/12/17.
//
#include <iostream>
#include <elfio/elfio.hpp>

void printElfInfo(const char* filePath) {
    ELFIO::elfio reader;

    // 读取 ELF 文件
    if (!reader.load(filePath)) {
        std::cerr << "Failed to load ELF file." << std::endl;
        return;
    }

    // 获取 ELF 文件头信息
    std::cout << "ELF Header Information:" << std::endl;

    // 输出文件类型
    std::cout << "File type: ";
    switch (reader.get_type()) {
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

    // 输出架构类型
    std::cout << "Architecture: ";
    if (reader.get_class() == ELFIO::ELFCLASS64) {
        std::cout << "64-bit" << std::endl;
    } else {
        std::cout << "32-bit" << std::endl;
    }

    // 输出入口点
    std::cout << "Entry point address: 0x" << std::hex << reader.get_entry() << std::endl;

    // 输出节区信息
    std::cout << "Section headers:" << std::endl;
    for (size_t i = 0; i < reader.sections.size(); ++i) {
        ELFIO::section* sec = reader.sections[i];
        std::cout << "  Section " << i << ": " << sec->get_name() << std::endl;
    }

    // 获取符号表节 (.symtab)
    ELFIO::section* symtab_section = reader.sections[".symtab"];
    if (!symtab_section) {
        std::cerr << "No symbol table found in ELF file." << std::endl;
        return;
    }

    // 创建 symbol_section_accessor 来访问符号表
    ELFIO::symbol_section_accessor symbols(reader, symtab_section);

    // 获取符号的数量
    size_t num_symbols = symbols.get_symbols_num();
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
        if (symbols.get_symbol(i, name, value, size, bind, type, section_index, other)) {
            // 输出符号的详细信息
            std::cout << "Symbol " << i << ": " << std::endl;
            std::cout << "  Name: " << name << std::endl;
//            printf(" Value: %p\n", value);
            std::cout << "  Value: " << std::hex << value << std::dec << std::endl;
            std::cout << "  Size: " << size << std::endl;
            std::cout << "  Bind: " << (int)bind << std::endl;
            std::cout << "  Type: " << (int)type << std::endl;
            std::cout << "  Section Index: " << section_index << std::endl;
            std::cout << "  Other: " << (int)other << std::endl;
            std::cout << "---------------------------------" << std::endl;
        }
    }

}

int main() {
    const char* filePath = "/tmp/tmp.5HaE5LCJSb/cmake-build-release-remote-host/cppFix";  // 修改为你要读取的 ELF 文件路径
    printElfInfo(filePath);
    return 0;
}
