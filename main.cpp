//
// Created by kwai on 2024/12/17.
//
#include <iostream>
void func(){

};
int global_a = 0;
const int const_b = 1;
int main(){
    printf(".data global_a = %p\n", &global_a);
    printf(".data const_b = %p\n", &const_b);
    printf(".text func = %p\n", &func);
    int stack_c = 1;
    printf(".stack stack_c = %p\n", &stack_c);
    printf("hello world!!!\n");
    return 0;
}