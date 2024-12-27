//
// Created by kwai on 2024/12/19.
//

#ifndef CPPFIX_HOTFIX_SIGNAL_H
#define CPPFIX_HOTFIX_SIGNAL_H
#include <csignal>
#include <iostream>
#include <unistd.h>

namespace hotfix_signal{
    enum HOTFIX_TYPE{
        HOOK = 1,
        UNHOOK,
    };
    struct signal_info{
        uint8_t hotfix_type = HOTFIX_TYPE::HOOK; // 代表 是hook 还是 unHook
        char cur_hotfix_name[256]{}; //当前hotfix的名字 需要唯一
        char cur_patch_so_path[256]{};  // so的路径
        char old_func_symbol_name[256]{}; // 旧函数的符号名字
        char new_func_symbol_name[256]{}; // 新函数的符号名字
    };
    const int HOTFIX_SIGNAL_VAL = SIGRTMIN + 1; // 自定义一个信号
    void signal_handler(int signal, siginfo_t* info, void* context);
    void register_signal();
    void wait_signal();
    void send_signal(const std::string &user_data);
    void test_send_and_receive_signal();
}

#endif //CPPFIX_HOTFIX_SIGNAL_H
