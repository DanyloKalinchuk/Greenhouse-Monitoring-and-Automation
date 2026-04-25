#include "ipc.hpp"

void IPC::ipc_handling(){
    while (this->ipc_on.load()){
        this->handle_msg();
    }
}

void IPC::ipc_write(uint16_t msg){
    uint8_t* msg_ptr = reinterpret_cast<uint8_t*>(&msg);
    int8_t bytes_to_write = sizeof(msg);

    while (bytes_to_write > 0){
        int8_t bytes_written = write(this->cfd, msg_ptr, bytes_to_write);

        if (bytes_written <= 0){
            throw std::runtime_error("Failed to SEND IPC message");
        }

        bytes_to_write -= bytes_written;
        msg_ptr += bytes_written;
    }
}

uint16_t IPC::ipc_read(){
    uint16_t msg;
    uint8_t* msg_ptr = reinterpret_cast<uint8_t*>(&msg);
    int8_t bytes_to_read = sizeof(msg);

    while (bytes_to_read > 0){
        int8_t bytes_read = read(this->cfd, msg_ptr, bytes_to_read);

        if (bytes_read <= 0){
            throw std::runtime_error("Failed to READ IPC message");
        }

        bytes_to_read -= bytes_read;
        msg_ptr += bytes_read;
    }

    return msg;
}

void IPC::handle_msg(){
    std::vector<uint16_t> buff;
    uint16_t msg = this->ipc_read();

    if (msg == MSG_CONF){
        for (int i = 0; i < 8; i++){
            buff.push_back(this->ipc_read());
        }

        this->handle_configuration(buff);
    }else if (msg == MSG_REQ){
        this->handle_data_request();
    }
}

void IPC::handle_configuration(std::vector<uint16_t> params){
    this->env_control.set_param(ENV_TEMPERATURE, params[0], params[1]);
    this->env_control.set_param(ENV_HUMIDITY, params[2], params[3]);
    this->env_control.set_param(ENV_MOISTURE, params[4], params[5]);
    this->env_control.set_param(ENV_CO2, params[6], params[7]);
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
    std::make_unique<Actuator>(10, 10),
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

    this->cfd = accept(this->sfd, NULL, NULL);
    if (this->cfd == -1){
        close(this->sfd);
        close(this->cfd);
        throw std::runtime_error("Failed to accept");
    }

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