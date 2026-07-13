#ifndef ACTUATOR_MANAGER_HPP
#define ACTUATOR_MANAGER_HPP

#include "../actuator/actuator.hpp"
#include "../../radio_comm/radio_comm.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <vector>

/// \brief Environmental parameters 
enum EnvParams{
    ENV_TEMPERATURE,
    ENV_HUMIDITY,
    ENV_MOISTURE,
    ENV_CO2
};

class ActuatorManager{
    std::thread manager_thread;
    std::atomic<bool> manager_on;
    std::mutex manager_mtx;
    std::condition_variable cv;
    std::atomic<bool> changes_occured = false;

    std::unique_ptr<Actuator> temp_act;
    std::atomic<uint32_t> temp;

    std::unique_ptr<Actuator> hum_act;
    std::atomic<uint32_t> humidity;

    std::unique_ptr<Actuator> soil_moist_act;
    std::atomic<uint32_t> soil_moist;

    std::unique_ptr<Actuator> co2_act;
    std::atomic<uint32_t> co2;

    std::mutex config_mtx;

    static std::string config_to_string(std::vector<uint16_t>& params);

    protected:
    void handle_actuator(EnvParams env_param);
    void handle_changes();

    public:
    ActuatorManager(
        std::unique_ptr<Actuator> temp_act,
        std::unique_ptr<Actuator> hum_act,
        std::unique_ptr<Actuator> soil_moist_act,
        std::unique_ptr<Actuator> co2_act
    );

    ~ActuatorManager();

    void update_parameters(SENS_FRAME frame);
    void update_configs(std::vector<uint16_t>& params);
};

#endif