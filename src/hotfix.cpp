//
// Created by kwai on 2024/12/18.
//
#include <hotfix.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <symbol_manager.h>

unsigned char jump_code[12] = {
        0x48, 0xB8,                         // mov rax, <new_func_addr>
        0, 0, 0, 0, 0, 0, 0, 0,             // 占位符，用于存储 new_func_addr
        0xFF, 0xE0                          // jmp rax
};

extern Symbol_manager* symbol_manager;
void Hotfix::hook()
{
    size_t page_size = getpagesize();
    size_t page_mask = (~(page_size - 1));
    uintptr_t aligned_addr = _old_func_addr & page_mask;  // 计算页面对齐地址
    size_t modify_page_num = 1;  // 改动小于1页，所以这里可以写死，只是set 12个字节
    mprotect((void *)aligned_addr, modify_page_num * page_size, PROT_READ|PROT_WRITE|PROT_EXEC);
    // 构造跳转指令
    // 将 _new_func_addr 填入跳转指令
    std::memcpy(&jump_code[2], &_new_func_addr, sizeof(_new_func_addr)); // 这里需要理解一下，拷贝的是这个地址值，而不是地址指向的内容！！！
    // 把原始内容拷贝过来
    if (std::memcpy(_old_func_addr_original_code, reinterpret_cast<void*>(_old_func_addr), sizeof(_old_func_addr_original_code)) == nullptr) {
        perror("memcpy _old_func_addr_original_code failed");
        return;
    }
    // 写入内存
    if (std::memcpy(reinterpret_cast<void*>(_old_func_addr), jump_code, sizeof(jump_code)) == nullptr) {
        perror("memcpy jump_code failed");
        return;
    }
    // 恢复页面权限为只读执行
    if (mprotect((void *)(aligned_addr), modify_page_num * page_size, PROT_READ | PROT_EXEC) != 0) {
        perror("mprotect restore failed");
    }
    // 输出成功信息
    std::cout << _hotfix_name << " hook success!!!" << std::endl;
}

void Hotfix::unHook()
{
    size_t page_size = getpagesize();
    size_t page_mask = (~(page_size - 1));
    uintptr_t aligned_addr = _old_func_addr & page_mask;  // 计算页面对齐地址
    size_t modify_page_num = 1;  // 改动小于1页，所以这里可以写死，只是set 12个字节
    mprotect((void *)aligned_addr, modify_page_num * page_size, PROT_READ|PROT_WRITE|PROT_EXEC);
    // 恢复旧函数的原始指令
    if (std::memcpy(reinterpret_cast<void*>(_old_func_addr), _old_func_addr_original_code, sizeof(_old_func_addr_original_code)) == nullptr) {
        perror("memcpy failed");
        return;
    }
    // 恢复页面权限为只读执行
    if (mprotect((void *)(aligned_addr), modify_page_num * page_size, PROT_READ | PROT_EXEC) != 0) {
        perror("mprotect restore failed");
    }
    // 输出成功信息
    std::cout << _hotfix_name << " unHook suc!!!" << std::endl;
}

void Hotfix::print_info()
{
    printf("hotfix_name = %s, old_func_addr = %p, new_func_addr = %p, old_func_symbol_name = %s, new_func_symbol_name = %s\n",
           _hotfix_name.c_str(), _old_func_addr, _new_func_addr, _old_func_symbol_name.c_str(), _new_func_symbol_name.c_str());
}


void Hotfix_manager::add_hotfix_and_execute(const std::string& hotfix_name, const std::string &patch_so_name, const std::string &old_func_symbol_name, const std::string& new_func_symbol_name) {
    ELFIO::Elf64_Addr old_func_addr = symbol_manager ->get_addr_by_name(old_func_symbol_name);
    ELFIO::Elf64_Addr new_func_addr = symbol_manager ->get_addr_from_some_so(patch_so_name, new_func_symbol_name);
    Hotfix *cur_hotfix = new Hotfix(hotfix_name, old_func_addr, new_func_addr, old_func_symbol_name, new_func_symbol_name);
    cur_hotfix->hook();
    hotfix_table[hotfix_name] = cur_hotfix;
}

void Hotfix_manager::recall_some_hotfix(const std::string &hotfix_name) {
    if(hotfix_table.find(hotfix_name) == hotfix_table.end())
    {
        std::cout << "cur hotfix does not exist" << std::endl;
        return;
    }
    hotfix_table[hotfix_name]->unHook();
    hotfix_table.erase(hotfix_name);
}

