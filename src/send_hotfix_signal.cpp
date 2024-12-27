//
// Created by kwai on 2024/12/19.
//
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <hotfix_signal.h>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

// 传递当前 patch的so 路径，共享内存的id
void send_signal(const pid_t cur_pid, const int shm_id) {
    std::cout << "send hotfix signal... " << std::endl;
    union sigval value;
    value.sival_int = shm_id; // 传递共享内存的id
    if (sigqueue(cur_pid, hotfix_signal::HOTFIX_SIGNAL_VAL, value) == 0) {
        std::cout << "Signal sent to PID: " << cur_pid << " with shm id: " << shm_id << std::endl;
    } else {
        perror("Failed to send signal");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <opcode> <target_pid> [additional_args...]" << std::endl;
        std::cerr << "Opcode 1: hotfix hook <hotfix_name> <current_patch_so_path> <old_func_symbol_name> <new_func_symbol_name>" << std::endl;
        std::cerr << "Opcode 2: hotfix unHook <hotfix_name>" << std::endl;
        return 1;
    }

    uint8_t hotfix_type = std::stoi(argv[1]); // 获取操作码
    pid_t target_pid = static_cast<pid_t>(std::stoi(argv[2])); // 获取目标 PID

    if (hotfix_type == hotfix_signal::HOTFIX_TYPE::HOOK) {
        // Hook 功能
        if (argc != 7) {
            std::cerr << "Opcode 1 requires: <hotfix_name> <current_patch_so_path> <old_func_symbol_name> <new_func_symbol_name>" << std::endl;
            return 1;
        }

        // 创建共享内存
        int shm_id = shmget(IPC_PRIVATE, sizeof(hotfix_signal::signal_info), IPC_CREAT | 0666);
        if (shm_id < 0) {
            perror("shmget failed");
            return 1;
        }

        // 将数据写入共享内存
        hotfix_signal::signal_info* shm_ptr = static_cast<hotfix_signal::signal_info*>(shmat(shm_id, nullptr, 0));
        if (shm_ptr == (hotfix_signal::signal_info*)-1) {
            perror("shmat failed");
            return 1;
        }
        shm_ptr -> hotfix_type = hotfix_type;
        // 填充共享内存数据
        strncpy(shm_ptr->cur_hotfix_name, argv[3], sizeof(shm_ptr->cur_hotfix_name) - 1);
        strncpy(shm_ptr->cur_patch_so_path, argv[4], sizeof(shm_ptr->cur_patch_so_path) - 1);
        strncpy(shm_ptr->old_func_symbol_name, argv[5], sizeof(shm_ptr->old_func_symbol_name) - 1);
        strncpy(shm_ptr->new_func_symbol_name, argv[6], sizeof(shm_ptr->new_func_symbol_name) - 1);

        // 发送信号执行 Hook
        send_signal(target_pid, shm_id);
    } else if (hotfix_type == hotfix_signal::HOTFIX_TYPE::UNHOOK) {
        // unHook 功能
        if (argc != 4) {
            std::cerr << "Opcode 2 requires: <hotfix_name>" << std::endl;
            return 1;
        }

        // 创建共享内存
        int shm_id = shmget(IPC_PRIVATE, sizeof(hotfix_signal::signal_info), IPC_CREAT | 0666);
        if (shm_id < 0) {
            perror("shmget failed");
            return 1;
        }

        // 将数据写入共享内存
        hotfix_signal::signal_info* shm_ptr = static_cast<hotfix_signal::signal_info*>(shmat(shm_id, nullptr, 0));
        if (shm_ptr == (hotfix_signal::signal_info*)-1) {
            perror("shmat failed");
            return 1;
        }
        shm_ptr -> hotfix_type = hotfix_type;
        // 填充共享内存数据（只需要 hotfix_name）
        strncpy(shm_ptr->cur_hotfix_name, argv[3], sizeof(shm_ptr->cur_hotfix_name) - 1);

        // 清空无关字段
        shm_ptr->cur_patch_so_path[0] = '\0';
        shm_ptr->old_func_symbol_name[0] = '\0';
        shm_ptr->new_func_symbol_name[0] = '\0';

        // 发送信号执行 unHook
        send_signal(target_pid, shm_id);
    } else {
        std::cerr << "Invalid opcode: " << hotfix_type << std::endl;
        return 1;
    }

    return 0;
}
