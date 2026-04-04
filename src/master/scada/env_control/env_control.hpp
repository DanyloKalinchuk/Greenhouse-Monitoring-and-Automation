#ifndef ENV_CONTROL
#define ENV_CONTROL

#include "../radio_comm/radio_comm.hpp"
#include <chrono>
#include <cstdint>
#include <thread>
#include <atomic>
#include <map>
#include <utility>

/*
    X_perf - stands for the perfect value of the environment parameter.
    X_error - stands for the difference between the upper 
        and lower limit of the parameter and the corresponding X_perf value.

    Units of meassurements
    | temp_X | Celsius | Temperature  |
    | hum_X  | RH      | Air Humidity |
*/

enum EnvParams{
    ENV_TEMPERATURE,
    ENV_HUMIDITY,
    ENV_MOISTURE,
    ENV_CO2
};

class EnvControl{
    std::thread ctr_thread;
    std::atomic<bool> ctr_on;
    std::thread comm_thread;
    std::atomic<bool> comm_on;

    Radio radio = Radio();
    SENS_FRAME frame_buff;
    std::map<uint8_t, std::pair<SENS_FRAME, uint64_t>> last_records;

    int16_t temp_perf = 20;
    uint8_t temp_error = 2;

    int16_t hum_perf = 40;
    uint8_t hum_error = 10;

    int16_t moist_perf = 40;
    uint8_t moist_error = 10;

    int16_t co2_perf = 40;
    uint8_t co2_error = 10;

    void handle_ctr();
    void handle_comm();

    void change_parameter(EnvParams env_param, bool increase);
    void check_params(SENS_FRAME frame);

    public:
    EnvControl();
    ~EnvControl();

    void set_param(EnvParams env_param, int16_t X_perf, uint8_t X_error);
    std::map<uint8_t, std::pair<SENS_FRAME, uint64_t> get_last_records();
};

#endif