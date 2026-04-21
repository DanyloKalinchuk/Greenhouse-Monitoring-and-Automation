#include "ipc/ipc.hpp"
#include <thread>
#include <chrono>

int main(){
    IPC ipc = IPC();

    while(1){
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}