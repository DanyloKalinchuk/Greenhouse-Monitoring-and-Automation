#include "env_control.hpp"

void EnvControl::handle_comm(){
    while (this->comm_on.load()){
        SENS_FRAME frame = this->radio.handle_communications();

        if (frame.sensor_id != DEFAULT_ID){
            std::chrono::time_point now = std::chrono::system_clock::now();
            std::lock_guard<std::mutex> last_rec_lock(this->last_rec_mtx);
            this->last_records[frame.sensor_id] = {frame, std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count()};
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
    std::unique_ptr<Actuator> moist_act, std::unique_ptr<Actuator> co2_act) : radio()
{
    this->temp_act = std::move(temp_act);
    this->hum_act = std::move(hum_act);
    this->moist_act = std::move(moist_act);
    this->co2_act = std::move(co2_act);
    

    this->comm_on.store(true);
    this->comm_thread = std::thread(&EnvControl::handle_comm, this);
}

EnvControl::EnvControl() : radio(0) {
    this->temp_act = std::make_unique<Actuator>(10, 10);
    this->hum_act = std::make_unique<Actuator>(10, 10);
    this->moist_act = std::make_unique<Actuator>(10, 10);
    this->co2_act = std::make_unique<Actuator>(10, 10);
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

std::vector<SENS_FRAME> EnvControl::get_last_records(){
    std::chrono::time_point now = std::chrono::system_clock::now();
    uint64_t secs_now = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::vector<SENS_FRAME> active_sens_data;

    std::lock_guard<std::mutex> last_rec_lock(this->last_rec_mtx);
    for (const auto& [sens_id, frame_n_timestamp] : this->last_records){
        if ((secs_now - frame_n_timestamp.second) <= ACTIVE_TIME_LIMIT_SEC){
            active_sens_data.push_back(frame_n_timestamp.first);
        }
    }

    return active_sens_data;
}

void EnvControl::set_last_records(std::vector<SENS_FRAME> frames){
    std::chrono::time_point now = std::chrono::system_clock::now();
            std::lock_guard<std::mutex> last_rec_lock(this->last_rec_mtx);
            
            for (const SENS_FRAME& frame : frames){
                this->last_records[frame.sensor_id] = {frame, std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count()};
            }
}
