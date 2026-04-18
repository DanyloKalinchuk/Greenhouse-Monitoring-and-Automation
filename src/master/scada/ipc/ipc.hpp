#ifndef IPC_HPP
#define IPC_HPP

#include "../env_control/env_control.hpp"
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <map>
#include <vector>
#include <utility>
#include <cstdint>
#include <thread>
#include <atomic>
#include <stdexcept>

#define MSG_CONF (1)
#define MSG_REQ (2)

#define CONF_BUFF_SIZE (8)

#define SOCKET_PATH ("/tmp/SCADA_SOCK")

class IPC{
    std::thread ipc_thread;
    std::atomic<bool> ipc_on;

    EnvControl env_control = EnvControl(
        std::make_unique<Actuator>(10, 10),
        std::make_unique<Actuator>(10, 10),
        std::make_unique<Actuator>(10, 10),
        std::make_unique<Actuator>(10, 10)
    );

    int sfd, cfd;
    struct sockaddr_un addr;

    void ipc_handling();
    void handle_configuration(std::vector<uint16_t> params);
    void handle_data_request();

    protected:
    void handle_msg();
    virtual void ipc_write(uint16_t msg);
    virtual uint16_t ipc_read();

    public:
    IPC();
    IPC(uint8_t dummy);
    ~IPC();

};

#endif