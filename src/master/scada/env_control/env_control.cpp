#include "env_control.hpp"

void EnvControl::handle_ctr(){
    while (this->thread_on.load()){
        if (this->frame_buff.sensor_id != DEFAULT_ID){
            SENS_FRAME frame = this->frame_buff;
            this->frame_buff.sensor_id = DEFAULT_ID;

            this->last_records[frame.sensor_id] = frame;
            this->check_params(frame);
        }
    }
}

void EnvControl::handle_comm(){
    while (this->comm_on.load()){
        if (this->frame_buff.sensor_id == DEFAULT_ID){
            this->frame_buff = this->radio.handle_communications();
        }
    }
}

EnvControl::EnvControl(){
    this->ctr_on.store(true);
    this->ctr_thread = std::thread(&EnvControl::handle_ctr, this);

    this->comm_on.store(true);
    this->comm_thread = std::thread(&EnvControl::handle_comm, this);
}

EnvControl::~EnvControl(){
    this->ctr_on.store(false);
    if (this->ctr_thread.joinable()){
        this->ctr_thread.join();
    }

    this->comm_on.store(false);
    if (this->comm_thread.joinable()){
        this->comm_thread.join();
    }
}

void EnvControl::set_param(EnvParams env_param, int16_t X_perf, uint8_t X_error){
    switch (env_param){
        case ENV_TEMPERATURE:
            this->temp_perf.store(X_perf);
            this->temp_error.store(X_error);
            break;
        case ENV_HUMIDITY:
            this->hum_perf.store(X_perf);
            this->hum_error.store(X_error);
            break;
    }
}

map<uint8_t, SENS_FRAME> EnvControl::get_last_records(){
    return this->last_records;
}
