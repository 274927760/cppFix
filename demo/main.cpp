#include <iostream>
#include <thread>
#include <dlfcn.h>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>


extern "C" int add(int a, int b)
{
    static int add_cnt = 0;
    add_cnt ++; 
    std::cout << add_cnt  << std::endl;
    return a + b;
}

void game() {
    while(1)
    {
        int a, b;
        std::cin >> a >> b;
        std::cout << add(a, b) << std::endl;
    }
}


void wait_so() {
    add(1,2);
    add(1,2);
    while(1) {
        std::string so_name, new_func_name, static_var_name;
        std::cout << "Enter so library path: ";
        std::getline(std::cin, so_name);
        std::cout << so_name << std::endl;

        void* handle = dlopen(so_name.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cout << "Failed to load shared library: " << dlerror() << std::endl;
            return;
        }
        std::cout << "please input new hotfix func name: ";
        std::getline(std::cin, new_func_name);
        std::cout << new_func_name << std::endl;
        std::cout << "please input static_var_name: ";
        std::cin >> static_var_name;
        std::cout << static_var_name << std::endl;
        typedef int (*compute_func)(int, int);
        void *new_func_addr = dlsym(handle, new_func_name.c_str());
        uintptr_t old_func_addr_int, add_cnt_addr_int;
        std::cout << "please_input_old_func_addr :";
        std::cin >> std::hex >> old_func_addr_int;
       std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
       std::cout << "please_input_old_static_var_addr: ";
       std::cin >> std::hex >>add_cnt_addr_int; 
        void *old_func_addr = (void *)(old_func_addr_int);
        void *add_cnt = (void *)(add_cnt_addr_int);
        void *static_var_addr = dlsym(handle, static_var_name.c_str());        
        if(!static_var_addr){

            std::cerr << "Failed to find static var : " << dlerror() << std::endl;
            return;
        }
        std::cout << "new_func_addr  = " << new_func_addr << std::endl;
        std::cout << "old_func_addr  = " << old_func_addr << std::endl;
        std::cout << "static_var_addr = " << static_var_addr << std::endl;
        size_t page_size = getpagesize();
        size_t page_mask = (~(page_size - 1));
        std::cout << page_size << " " << page_mask << std::endl;
        printf("%p\n",page_mask);
        
        size_t modify_page_num = 1;

        mprotect((void *)((unsigned long)old_func_addr & page_mask), modify_page_num * page_size, PROT_READ|PROT_WRITE|PROT_EXEC);
        //mprotect((void *)((unsigned long)add_cnt & page_mask),modify_page_num * page_size, PROT_READ);      
        mprotect((void *)((unsigned long)static_var_addr & page_mask),modify_page_num * page_size, PROT_READ|PROT_WRITE);

 
        memset((void *)old_func_addr, 0x48, 1);
        memset((void *)old_func_addr + 1, 0xb8, 1);
        memcpy((void *)old_func_addr + 2, &new_func_addr, 8);
        memset((void *)old_func_addr + 10, 0xff, 1);
        memset((void *)old_func_addr + 11, 0xe0, 1);
        

        memcpy(static_var_addr, add_cnt,4);
        std::cout << old_func_addr << std::endl;
        std::cout << "hotfix suc!!!" <<std::endl;
        game();
        dlclose(handle);
    }
}


int main(){
    wait_so();
    return 0;
}
