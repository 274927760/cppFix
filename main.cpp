//
// Created by kwai on 2024/12/17.
//
#include <iostream>
#include <elfio/elfio.hpp>
#include <elfio/elf_types.hpp>
#include <unistd.h>   // For readlink()
#include <limits.h>   // For PATH_MAX
#include <regex>
#include <symbol_manager.h>
#include <pmap.h>
#include <hotfix_signal.h>
#include <hotfix.h>
Symbol_manager* symbol_manager;
Hotfix_manager* hotfix_manager;
__attribute__((noinline)) int add(int a, int b)
{
    return a + b;
}

void game()
{
    while (true) {
        int a, b;
        std::cout << "please input (a,b): ";
        if (!(std::cin >> a >> b)) {    // 检查输入流的状态
            std::cout << "invalid input" << std::endl;
            std::cin.clear();   // 清除输入流的错误状态
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');   // 忽略之前错误的输入
            continue;
        }
        int res = add(a, b);
        std::cout << "res = " << res << std::endl;
    }
}

void init(){
    symbol_manager = new Symbol_manager();
    hotfix_manager = new Hotfix_manager();
}
int main() {
    printf("global_func addr = %p\n", &add);
    init();
//    symbol_manager->test_addr_terminal();
    hotfix_signal::register_signal();
//    hotfix_signal::test_send_and_receive_signal();
    game();
    return 0;
}
