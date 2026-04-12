#ifndef ENV_CONTROL
#define ENV_CONTROL

#include "../radio_comm/radio_comm.hpp"
#include "actuator/actuator.hpp"
#include <chrono>
#include <cstdint>
#include <thread>
#include <atomic>
#include <map>
#include <utility>
#include <memory>

#define TEMP_ACT_LINE 1
#define HUM_ACT_LINE 2
#define MOIST_ACT_LINE 3
#define CO2_ACT_LINE 4

enum EnvParams{
    ENV_TEMPERATURE,
    ENV_HUMIDITY,
    ENV_MOISTURE,
    ENV_CO2
};

class EnvControl{
    Radio radio = Radio();
    std::map<uint8_t, std::pair<SENS_FRAME, uint64_t>> last_records;

    std::thread comm_thread;

    protected:
    std::atomic<bool> comm_on;

    std::unique_ptr<Actuator> temp_act;
    std::unique_ptr<Actuator> hum_act;
    std::unique_ptr<Actuator> moist_act;
    std::unique_ptr<Actuator> co2_act;

    void change_parameter(SENS_FRAME frame);
    virtual void handle_comm();

    public:
    EnvControl(std::unique_ptr<Actuator> temp_act, std::unique_ptr<Actuator> hum_act, 
        std::unique_ptr<Actuator> moist_act, std::unique_ptr<Actuator> co2_act);
    ~EnvControl();

    /*
    X_perf - stands for the perfect value of the environment parameter.
    X_error - stands for the difference between the upper 
        and lower limit of the parameter and the corresponding X_perf value.
    */
    void set_param(EnvParams env_param, int16_t X_perf, uint8_t X_error);
    std::map<uint8_t, std::pair<SENS_FRAME, uint64_t>> get_last_records();
};

#endif