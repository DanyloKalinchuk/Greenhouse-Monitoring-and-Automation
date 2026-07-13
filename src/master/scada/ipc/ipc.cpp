#include "ipc.hpp"

void IPC::ipc_handling(){
    while (this->ipc_on.load()){
        if (this->disconnected.load()){
            Logger::get_instance()->log_out(IPC_LOGS, "Listening ...", MsgType::INFO);

            int event = poll(this->fds, POLLFD_SIZE, POLL_TIMEOUT);
            if (!event) continue;

            this->cfd = accept(this->sfd, NULL, NULL);
            if (this->cfd == -1){
                close(this->sfd);
                close(this->cfd);
                throw std::runtime_error("Failed to accept");
            }
            this->fds[1].fd = this->cfd;
            this->disconnected.store(false);

            Logger::get_instance()->log_out(IPC_LOGS, "Connected", MsgType::INFO);
        }

        this->handle_msg();
    }
}

void IPC::ipc_write(uint16_t msg){
    if (write(this->cfd, &msg, sizeof(msg)) < sizeof(msg)){
        Logger::get_instance()->log_out(IPC_LOGS, "ipc_write() failed", MsgType::ERROR);

        throw std::runtime_error("Failed to SEND IPC message");
    }
}

uint16_t IPC::ipc_read(){
    int event = poll(this->fds, POLLFD_SIZE, POLL_TIMEOUT);
    if (!event) return MSG_TIMEOUT;

    uint16_t msg;
    int32_t bytes_read = recv(this->cfd, &msg, sizeof(msg), MSG_NOSIGNAL);

    if (bytes_read == 0){
        Logger::get_instance()->log_out(IPC_LOGS, "Disconnected", MsgType::WARNING);

        this->disconnected.store(true);
        msg = MSG_DISC;
    }else if (bytes_read < 0){
        Logger::get_instance()->log_out(IPC_LOGS, "ipc_read() failed", MsgType::ERROR);

        throw std::runtime_error("Failed to READ IPC message");
    }

    return msg;
}

void IPC::handle_msg(){
    std::vector<uint16_t> buff;
    uint16_t msg = this->ipc_read();

    if (msg == MSG_CONF){
        Logger::get_instance()->log_out(IPC_LOGS, "Received configuration message", MsgType::INFO);

        for (int i = 0; i < 8; i++){
            buff.push_back(this->ipc_read());
        }

        this->handle_configuration(buff);
    }else if (msg == MSG_REQ){
        Logger::get_instance()->log_out(IPC_LOGS, "Received data request message", MsgType::INFO);

        this->handle_data_request();
    }
}

void IPC::handle_configuration(std::vector<uint16_t> params){
    std::thread config_update([this, params] {this->env_control.set_params(params);});
    config_update.detach();
}

void IPC::handle_data_request(){
    std::vector<SENS_FRAME> frames = this->env_control.get_last_records();
    uint16_t msg = frames.size();
    this->ipc_write(msg);

    for (SENS_FRAME& frame : frames){
        msg = frame.sensor_id;
        this->ipc_write(msg);

        msg = frame.temperature;
        this->ipc_write(msg);
        msg = frame.humidity;
        this->ipc_write(msg);
        msg = frame.soil_moisture;
        this->ipc_write(msg);
        msg = frame.co2;
        this->ipc_write(msg);
    }
}

IPC::IPC() : env_control(
    std::make_unique<ActuatorServo>(std::make_unique<PWMLine>(PWM0, PERIOD, POS_INIT), 20, 5),
    std::make_unique<Actuator>(10, 10),
    std::make_unique<Actuator>(10, 10),
    std::make_unique<Actuator>(10, 10)
) {
    this->sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (this->sfd == -1){
        throw std::runtime_error("Failed to open the socket");
    }

    if (remove(SOCKET_PATH) == -1 && errno != ENOENT){
        throw std::runtime_error(std::string("Unnable to remove ") + SOCKET_PATH);
    }

    memset(&this->addr, 0, sizeof(struct sockaddr_un));
    this->addr.sun_family = AF_UNIX;
    strcpy(this->addr.sun_path, SOCKET_PATH);

    if (bind(this->sfd, (struct sockaddr *) (&this->addr), sizeof(struct sockaddr_un)) == -1){
        throw std::runtime_error("Failed to bind the socket");
    }

    if (listen(this->sfd, 1) == -1){
        throw std::runtime_error("Failed to listen");
    }

    this->disconnected.store(true);

    this->fds[0].fd = this->sfd;
    this->fds[0].events = POLLIN;

    this->fds[1].fd = -1;
    this->fds[1].events = POLLIN;

    this->ipc_on.store(true);
    this->ipc_thread = std::thread(&IPC::ipc_handling, this);
}

IPC::IPC(uint8_t dummy) : env_control() {
    this->sfd = -1;
}

IPC::~IPC(){
    this->ipc_on.store(false);
    if (this->ipc_thread.joinable()){
        this->ipc_thread.join();
    }

    if (this->sfd != -1){
        close(this->sfd);
        close(this->cfd);
    }
}