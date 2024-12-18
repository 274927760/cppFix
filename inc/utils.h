//
// Created by kwai on 2024/12/18.
//

#ifndef CPPFIX_UTILS_H
#define CPPFIX_UTILS_H

#include <iostream>
// 定义一些规则，判断是否为用户自定义的一些符号
bool is_user_defined_symbol(const std::string& name) {
    return !((name.rfind("_ZSt", 0) == 0) or (name.rfind("_ZNSt", 0) == 0));
}


#endif //CPPFIX_UTILS_H
