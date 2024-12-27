//
// Created by kwai on 2024/12/19.
//

#include "hotfix_signal.h"
#include <thread>
#include <cstring>
#include <sys/shm.h>
#include <symbol_manager.h>
#include <hotfix.h>

extern Symbol_manager* symbol_manager;
extern Hotfix_manager* hotfix_manager;
namespace hotfix_signal{
    // 信号处理函数
    void signal_handler(int signal, siginfo_t* info, void* context) {
        if (signal == HOTFIX_SIGNAL_VAL) {
            int shm_id = info->si_value.sival_int; // 获取共享内存 ID
            printf("Received SIGUSR1 from pid = %d, shm_id = %d\n", info->si_pid, shm_id);
            signal_info* shm_ptr = static_cast<signal_info*>(shmat(shm_id, nullptr, 0)); // 附加共享内存
            if (shm_ptr == (signal_info*)-1) {
                perror("shmat failed");
                return;
            }
            if(shm_ptr -> hotfix_type == hotfix_signal::HOTFIX_TYPE::HOOK)
            {
                hotfix_manager -> add_hotfix_and_execute(std::string(shm_ptr->cur_hotfix_name),
                                                         std::string(shm_ptr -> cur_patch_so_path),
                                                         std::string(shm_ptr -> old_func_symbol_name),
                                                         std::string(shm_ptr -> new_func_symbol_name));
            }
            else if(shm_ptr -> hotfix_type == hotfix_signal::HOTFIX_TYPE::UNHOOK){
                hotfix_manager->recall_some_hotfix(std::string(shm_ptr->cur_hotfix_name));
            }
            // 分离共享内存
            shmdt(shm_ptr);
            // 删除共享内存
            shmctl(shm_id, IPC_RMID, nullptr);
        }
    }

// 注册信号
    void register_signal(){
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;  // 使用扩展信息
        sa.sa_sigaction = signal_handler;
        // 注册实时信号处理
        sigaction(HOTFIX_SIGNAL_VAL, &sa, nullptr);
        std::cout << "cur pid = " << getpid() << ", register_signal " << HOTFIX_SIGNAL_VAL << ", suc!!! " << std::endl;
    }

    void wait_signal(){
        std::cout << "Waiting for hotfix signals..." << std::endl;
        while (true) {
            pause(); // 等待信号
        }
    }

    // 测试发送和接收信号
    void test_send_and_receive_signal() {
        // 启动发送信号的线程
//        std::thread send_signal_thread([]() {
//            while(true)
//            {
//                sleep(1); // 延迟 1 秒确保接收线程已经启动
//                hotfix_signal::send_signal("hello world");
//            }
//        });

        // 主线程等待信号
//        wait_signal();
        // 等待发送线程完成
//        send_signal_thread.join();
    }

    void send_signal(const std::string &user_data){
        std::cout << "send hotfix signal... " << std::endl;
        pid_t self_pid = getpid(); // 获取当前进程的 PID
        union sigval value;
        char* data_ptr = new char[user_data.length() + 1]; // +1 for null terminator
        std::strcpy(data_ptr, user_data.c_str()); // 复制字符串到动态内存
        value.sival_ptr = const_cast<char*>(user_data.c_str()); // 附加数据
            if (sigqueue(self_pid, HOTFIX_SIGNAL_VAL, value) == 0) {
            std::cout << "Signal sent to self (PID: " << self_pid << ") with data: " << user_data << std::endl;
        } else {
            perror("Failed to send signal");
        }
    }
}


