//
// Created by kwai on 2024/12/18.
//

#ifndef CPPFIX_PMAP_H
#define CPPFIX_PMAP_H

#include <iostream>
#include <elfio/elfio.hpp>

class Process_map_manager {
public:
    Process_map_manager() = default;
    ~Process_map_manager() = default;
    Process_map_manager(const std::string &elf_path);
    void load_cur_proc_mem_maps(const std::string &elf_path);
    void print_memory_mappings();
    ELFIO::Elf64_Addr get_code_start_addr(){
        return code_start_addr;
    };
    ELFIO::Elf64_Addr get_data_start_addr(){
        return data_start_addr;
    }
private:
    ELFIO::Elf64_Addr code_start_addr, data_start_addr;
};


#endif //CPPFIX_PMAP_H
