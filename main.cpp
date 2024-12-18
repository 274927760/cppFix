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

int add(int a, int b)
{
    return a + b;
}

int mul(int a, int b)
{
    return a * b;
}
int main() {
    printf("global_func addr = %p\n", &add);
    Symbol_manager s = Symbol_manager();
    s.test_addr_terminal();
    return 0;
}
