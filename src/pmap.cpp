//
// Created by kwai on 2024/12/18.
//

#include "pmap.h"
#include <iostream>
#include <sstream>

Process_map_manager::Process_map_manager(const std::string &elf_path) {
    load_cur_proc_mem_maps(elf_path);
}

void Process_map_manager::load_cur_proc_mem_maps(const std::string &elf_path){
    std::ifstream maps_file("/proc/self/maps");
    if (!maps_file.is_open()) {
        std::cerr << "Failed to open /proc/self/maps" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(maps_file, line)) {
        std::istringstream iss(line);
        std::string start_addr, permissions, offset, dev, inode, pathname;
        iss >> start_addr >> permissions >> offset
            >> dev >> inode >> pathname;
        // 去掉所有空格
        if(pathname.size()) pathname.erase(std::remove_if(pathname.begin(), pathname.end(), ::isspace), pathname.end());
        if(pathname == elf_path)
        {
            size_t dash_pos = start_addr.find('-');
            if (dash_pos != std::string::npos) {
                start_addr = start_addr.substr(0, dash_pos);
            }
            if (permissions == "r-xp") // 可读 可执行代码段
            {
                code_start_addr = std::stoull(start_addr, 0, 16);
            }
            else if(permissions == "rw-p") // 可读 可写
            {
                data_start_addr = std::stoull(start_addr, 0, 16);
            }
        }
    }
}

void Process_map_manager::print_memory_mappings() {
    std::cout << "code_start_addr = " << std::hex << code_start_addr << ", data_start_addr = " << data_start_addr << std::dec << std::endl;
}
