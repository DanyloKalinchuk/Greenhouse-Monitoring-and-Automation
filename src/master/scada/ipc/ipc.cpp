#include "ipc.hpp"

void IPC::ipc_handling(){
    while (this->ipc_on.load()){
        
    }
}

IPC::IPC(){
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

IPC::~IPC(){
    this->ipc_on.store(false);
    if (this->ipc_thread.joinable()){
        this->ipc_thread.join();
    }

    close(this->sfd);
    close(this->cfd);
}