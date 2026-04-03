#include "radio_logs.hpp"

RadioLogs::RadioLogs(std::string path){
    this->log_file = std::fstream(path, std::ios::out | std::ios::app);

    if (!this->log_file.is_open()){
        throw std::runtime_error("Failed to open the logging file");
    }
}

RadioLogs::~RadioLogs(){
    this->log_file.close();
}

void RadioLogs::log_out(uint8_t initiator_id, MsgType msg_type){
    std::string log_msg = asctime(&this->datetime);
    
    log_msg += " | Initiator ID: ";
    log_msg += initiator_id;

    log_msg += " | Message type: ";
    switch (msg_type){
        case MasterStart:
            log_msg += "MasterStart\n";
            break;
        case MasterFail:
            log_msg += "MasterFail\n";
            break;
        case SensorInit:
            log_msg += "SensorInit\n";
            break;
        case SensorRead:
            log_msg += "SensorRead\n";
            break;
    }

    this->log_file << log_msg;
}