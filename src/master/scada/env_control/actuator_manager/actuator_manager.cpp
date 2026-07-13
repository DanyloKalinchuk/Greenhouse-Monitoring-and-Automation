#include "actuator_manager.hpp"

std::string ActuatorManager::config_to_string(std::vector<uint16_t>& params){
    std::string message = "Updared actuator configurations: \n";
    message += "\tTemperature: perf= " + std::to_string(params[0]) + ", error: " + \
        std::to_string(params[1]) + "\n";
    
    message += "\tHumidity: perf= " + std::to_string(params[2]) + ", error: " + \
        std::to_string(params[3]) + "\n";

    message += "\tSoil Moisture: perf= " + std::to_string(params[4]) + ", error: " + \
        std::to_string(params[5]) + "\n";

    message += "\tCO2: perf= " + std::to_string(params[6]) + ", error: " + \
        std::to_string(params[7]);

    return message;
}

void ActuatorManager::handle_actuator(EnvParams env_param){
    std::lock_guard<std::mutex> config_lock(this->config_mtx);

    switch (env_param){
        case EnvParams::ENV_TEMPERATURE:
            this->temp_act->handle_changes(this->temp.load());
            break;
        case EnvParams::ENV_HUMIDITY:
            this->hum_act->handle_changes(this->humidity.load());
            break;
        case EnvParams::ENV_MOISTURE:
            this->soil_moist_act->handle_changes(this->soil_moist.load());
            break;
        case EnvParams::ENV_CO2:
            this->co2_act->handle_changes(this->co2.load());
            break;
        default:
            break;
    }
}

void ActuatorManager::handle_changes(){
    while (this->manager_on.load()){
        std::unique_lock<std::mutex> manager_lock(this->manager_mtx);
        this->cv.wait(manager_lock, [this] {return this->changes_occured.load();});

        if (!this->manager_on.load()) return;
        this->changes_occured.store(false);

        this->handle_actuator(EnvParams::ENV_TEMPERATURE);
        this->handle_actuator(EnvParams::ENV_HUMIDITY);
        this->handle_actuator(EnvParams::ENV_MOISTURE);
        this->handle_actuator(EnvParams::ENV_CO2);
    }
}

ActuatorManager::ActuatorManager(
    std::unique_ptr<Actuator> temp_act,
    std::unique_ptr<Actuator> hum_act,
    std::unique_ptr<Actuator> soil_moist_act,
    std::unique_ptr<Actuator> co2_act
){
    this->temp_act = std::move(temp_act);
    this->hum_act = std::move(hum_act);
    this->soil_moist_act = std::move(soil_moist_act);
    this->co2_act = std::move(co2_act);

    this->manager_on.store(true);
    this->manager_thread = std::thread(&ActuatorManager::handle_changes, this);
}

ActuatorManager::~ActuatorManager(){
    this->manager_on.store(false);
    this->changes_occured.store(true);
    this->cv.notify_all();

    if (this->manager_thread.joinable()){
        this->manager_thread.join();
    }
}

void ActuatorManager::update_parameters(SENS_FRAME frame){
    this->temp.store(frame.temperature);
    this->humidity.store(frame.humidity);
    this->soil_moist.store(frame.soil_moisture);
    this->co2.store(frame.co2);
    
    this->changes_occured.store(true);
    this->cv.notify_all();
}

void ActuatorManager::update_configs(std::vector<uint16_t>& params){
    std::lock_guard<std::mutex> config_lock(this->config_mtx);

    this->temp_act->set_target(params[0], params[1]);
    this->hum_act->set_target(params[2], params[3]);
    this->soil_moist_act->set_target(params[4], params[5]);
    this->co2_act->set_target(params[6], params[7]);

    this->changes_occured.store(true);
    this->cv.notify_all();

    Logger::get_instance()->log_out(this->config_to_string(params), MsgType::INFO);
}