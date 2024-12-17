#include <stdio.h>
// 使用 visibility 属性导出符号
int hotfix_add_cnt __attribute__((visibility("default"))) = 0;
extern "C" int add_hotfix(int a, int b)
{
    puts("call add_hotfix");   
    printf("hotfix_add_cnt %d\n",hotfix_add_cnt);
    hotfix_add_cnt ++;
    return a * b;
}
