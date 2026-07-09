#ifndef IPC_HPP
#define IPC_HPP

#include "../env_control/env_control.hpp"
#include "../logger/logger.hpp"
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <unistd.h>
#include <chrono>
#include <map>
#include <vector>
#include <utility>
#include <cstdint>
#include <thread>
#include <atomic>
#include <stdexcept>

#define MSG_DISC (0) ///< Message used on client disconnect
#define MSG_CONF (1) ///< Configuration request message
#define MSG_REQ (2) ///< Data request message
#define MSG_TIMEOUT (3) ///< Message used on poll timeout

#define POLL_TIMEOUT (500) ///< Time time(ms) the thread will wait before waking up
#define POLLFD_SIZE (2)

#define CONF_BUFF_SIZE (8) ///< Number of values to be received during configuration request

#define SOCKET_PATH ("/tmp/SCADA_SOCK") ///< Path to the socket

#define IPC_LOGS ("logs/ipc_logs.log") ///< Path to the IPC logs output file

/// \brief Class for handling IPC
class IPC{
    std::thread ipc_thread;
    std::atomic<bool> ipc_on; ///< Keeps ipc_thread alive
    std::atomic<bool> disconnected; ///< Is set True on MSG_DISC 

    int sfd, cfd;
    struct sockaddr_un addr;
    struct pollfd fds[POLLFD_SIZE];

    /// \brief Opens the socket if disconnected is set. Handles IPC requests
    void ipc_handling();

    /**
     * \brief Handles MSG_CONF
     * 
     * \param params CONF_BUFF_SIZE values corresponding to perf and error values of Actuators
     */
    void handle_configuration(std::vector<uint16_t> params);

    /// \brief Handles MSG_REQ
    void handle_data_request();

    protected:
    EnvControl env_control;

    /// \brief Prepares data and calls handle routine correspondingly to a received request
    void handle_msg();

    /// \brief Sends one 16-bit value via IPC socket
    virtual void ipc_write(uint16_t msg);

    /// \brief Returns one 16-bit value received via IPC socket 
    virtual uint16_t ipc_read();

    public:
    IPC();
    /// \brief Constructs an IPC object without opening socket
    IPC(uint8_t dummy);
    ~IPC();

};

#endif