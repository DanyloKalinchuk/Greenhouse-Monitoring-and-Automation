#ifndef IPC_HPP
#define IPC_HPP

#include "../env_control/env_control.hpp"
#include <sys/un.h>
#include <sys/socket.h>
#include <chrono>
#include <map>
#include <utility>
#include <cstdint>
#include <thread>
#include <atomic>
#include <stdexcept>

#define SOCKET_PATH ("/tmp/SCADA_SOCK")

class IPC{
    std::thread ipc_thread;
    std::atomic<bool> ipc_on;

    int sfd, cfd;
    struct sockaddr_un addr;

    void ipc_handling();

    public:
    IPC();
    ~IPC();

};

#endif