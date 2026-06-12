#ifndef ENV_CONTROL
#define ENV_CONTROL

#include "actuator/actuator.hpp"
#include <chrono>
#include <cstdint>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <vector>
#include <utility>
#include <memory>

#ifdef RADIO_OPTION_RF24
#include "../radio_comm/radio_rf24.hpp"
#else
#include "../radio_comm/radio_ble.hpp"
#endif

#define ACTIVE_TIME_LIMIT_SEC 25 ///< 2.5 data readings on the sensor side

/// \brief Environmental parameters 
enum EnvParams{
    ENV_TEMPERATURE,
    ENV_HUMIDITY,
    ENV_MOISTURE,
    ENV_CO2
};

/// \brief Class for managing Actuators and last SENS_FRAMEs received
class EnvControl{
    std::unique_ptr<Radio> radio = nullptr;
    std::map<uint8_t, std::pair<SENS_FRAME, uint64_t>> last_records; ///< Holds last SENS_FRAME and the time when it was received for each registered Sensor

    std::thread comm_thread; ///< Radio communication thread
    std::mutex last_rec_mtx; ///< Mutex for last_records

    protected:
    std::atomic<bool> comm_on; ///< Keeps comm_thread alive

    std::unique_ptr<Actuator> temp_act;
    std::unique_ptr<Actuator> hum_act;
    std::unique_ptr<Actuator> moist_act;
    std::unique_ptr<Actuator> co2_act;

    /**
     * \brief Calls handle_changes routine of Actuators, passing corresponding values for each
     * 
     * \param frame SENS_FRAME structure containing the last received environmental parameter values
     */
    void change_parameter(SENS_FRAME frame);

    /// \brief Calls Radio::handle_communications() and handles received SENS_FRAMEs
    virtual void handle_comm();

    public:
    /// \brief Constructs EnvControl object with passed Actuators
    EnvControl(std::unique_ptr<Actuator> temp_act, std::unique_ptr<Actuator> hum_act, 
        std::unique_ptr<Actuator> moist_act, std::unique_ptr<Actuator> co2_act);

    /// \brief Construct EnvControl object with hardware-less Actuator instances
    EnvControl();
    ~EnvControl();

   /**
    * \brief Updates perf and error values of an Actuator
    * 
    * \param env_param EnvParams structure. Defines which Actuator's values to change
    * \param X_perf The perfect value of the environment parameter.
    * \param X_error The difference between the upper and lower limit of the parameter and the corresponding X_perf value.
    */
    void set_param(EnvParams env_param, int16_t X_perf, uint8_t X_error);
    
    /**
     * \brief Looks for SENS_FRAMEs from active Sensors
     * Sensor is identified as active if the last SENS_FRAME was received during the period of ACTIVE_TIME_LIMIT_SEC
     * 
     * \return Vector of last received SENS_FRAMEs from active sensors 
     */
    std::vector<SENS_FRAME> get_last_records();

    /// \brief Fills last_records vector with SENS_FRAMEs
    void set_last_records(std::vector<SENS_FRAME> frames);
};

#endif