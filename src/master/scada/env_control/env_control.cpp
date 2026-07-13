#include "env_control.hpp"

void EnvControl::handle_comm(){
    while (this->comm_on.load()){
        SENS_FRAME frame = this->radio->handle_communications();

        if (frame.sensor_id != DEFAULT_ID){
            std::chrono::time_point now = std::chrono::system_clock::now();
            std::lock_guard<std::mutex> last_rec_lock(this->last_rec_mtx);
            this->last_records[frame.sensor_id] = {frame, std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count()};
            this->actuator_manager->update_parameters(frame);
        } 
    }
}

/*
void EnvControl::change_parameter(SENS_FRAME frame){
    Logger::get_instance()->log_out(ENV_CTRL_LOGS, "Handling parameters changes...", MsgType::INFO);

    this->temp_act->handle_changes(frame.temperature);
    this->hum_act->handle_changes(frame.humidity);
    this->moist_act->handle_changes(frame.soil_moisture);
    this->co2_act->handle_changes(frame.co2);

    Logger::get_instance()->log_out(ENV_CTRL_LOGS, "Finished handling changes", MsgType::INFO);
}
*/

EnvControl::EnvControl(std::unique_ptr<Actuator> temp_act, std::unique_ptr<Actuator> hum_act, 
    std::unique_ptr<Actuator> moist_act, std::unique_ptr<Actuator> co2_act)
{
    Logger::get_instance()->log_out(ENV_CTRL_LOGS, "EnvControl starting...", MsgType::INFO);

    #ifdef RADIO_OPTION_RF24
        this->radio = std::make_unique<Radio_RF24>();
    #else
        this->radio = std::make_unique<Radio_BLE>();
    #endif

    this->actuator_manager = std::make_unique<ActuatorManager>(
        std::move(temp_act), std::move(hum_act),
        std::move(moist_act), std::move(co2_act)
    );
    

    this->comm_on.store(true);
    this->comm_thread = std::thread(&EnvControl::handle_comm, this);

    Logger::get_instance()->log_out(ENV_CTRL_LOGS, "EnvControl started", MsgType::INFO);
}

EnvControl::EnvControl(){
    Logger::get_instance()->log_out(ENV_CTRL_LOGS, "EnvControl starting...", MsgType::INFO);

    this->actuator_manager = std::make_unique<ActuatorManager>(
        std::make_unique<Actuator>(10, 10),
        std::make_unique<Actuator>(10, 10),
        std::make_unique<Actuator>(10, 10),
        std::make_unique<Actuator>(10, 10)
    );

    Logger::get_instance()->log_out(ENV_CTRL_LOGS, "EnvControl started", MsgType::INFO);
}

EnvControl::~EnvControl(){
    this->comm_on.store(false);
    if (this->comm_thread.joinable()){
        this->comm_thread.join();
    }
}

void EnvControl::set_params(std::vector<uint16_t> params){
    this->actuator_manager->update_configs(params);
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
