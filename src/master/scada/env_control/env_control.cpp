#include "env_control.hpp"

void EnvControl::handle_comm(){
    while (this->comm_on.load()){
        SENS_FRAME frame = this->radio.handle_communications();

        if (frame.sensor_id != DEFAULT_ID){
            std::chrono::time_point now = std::chrono::system_clock::now();
            this->last_records[frame.sensor_id] = {frame, std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()};
            this->change_parameter(frame);
        } 
    }
}

void EnvControl::change_parameter(SENS_FRAME frame){
    this->temp_act->handle_changes(frame.temperature);
    this->hum_act->handle_changes(frame.humidity);
    this->moist_act->handle_changes(frame.soil_moisture);
    this->co2_act->handle_changes(frame.co2);
}

EnvControl::EnvControl(std::unique_ptr<Actuator> temp_act, std::unique_ptr<Actuator> hum_act, 
    std::unique_ptr<Actuator> moist_act, std::unique_ptr<Actuator> co2_act)
{
    this->radio = Radio();

    this->temp_act = std::move(temp_act);
    this->hum_act = std::move(hum_act);
    this->moist_act = std::move(moist_act);
    this->co2_act = std::move(co2_act);

    this->comm_on.store(true);
    this->comm_thread = std::thread(&EnvControl::handle_comm, this);
}

EnvControl::~EnvControl(){
    this->comm_on.store(false);
    if (this->comm_thread.joinable()){
        this->comm_thread.join();
    }
}

void EnvControl::set_param(EnvParams env_param, int16_t X_perf, uint8_t X_error){
    switch (env_param){
        case ENV_TEMPERATURE:
            this->temp_act->set_target(X_perf, X_error);
            break;
        case ENV_HUMIDITY:
            this->hum_act->set_target(X_perf, X_error);
            break;
        case ENV_MOISTURE:
            this->moist_act->set_target(X_perf, X_error);
            break;
        case ENV_CO2:
            this->co2_act->set_target(X_perf, X_error);
            break;
    }
}

std::map<uint8_t, std::pair<SENS_FRAME, uint64_t>> EnvControl::get_last_records(){
    return this->last_records;
}
