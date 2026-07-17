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

/// \brief Class for managing Actuators
class ActuatorManager{
    std::thread manager_thread; ///< Main ActuatorManager thread
    std::atomic<bool> manager_on; ///< Keeps manager_thread alive
    std::mutex manager_mtx; ///< Used with the cv
    std::condition_variable cv; ///< Conditional variable for manager_thread
    std::atomic<bool> changes_occured = false; ///< Prevents spurious awakenings

    std::unique_ptr<Actuator> temp_act; ///< Actuator that regulates Temperature
    std::atomic<uint32_t> temp; ///< Last received Temperature value

    std::unique_ptr<Actuator> hum_act; ///< Actuator that regulates Humidity
    std::atomic<uint32_t> humidity; ///< Last received Humidity value

    std::unique_ptr<Actuator> soil_moist_act; ///< Actuator that regulates Soil Moisture
    std::atomic<uint32_t> soil_moist; ///< Last received Soil Moisture value

    std::unique_ptr<Actuator> co2_act; ///< Actuator that regulates CO2
    std::atomic<uint32_t> co2; ///< Last received CO2 value

    std::mutex config_mtx; ///< Prevents concurrent access to actuators perf and error values

    /**
     * \brief Converts vector of perf and error values for actuators into a string message
     */
    static std::string config_to_string(std::vector<uint16_t>& params);

    protected:
    /// \brief Passes received data to the corresponding actuator
    void handle_actuator(EnvParams env_param);

    /// \brief ActuatoManager main loop
    void handle_changes();

    public:
    ActuatorManager(
        std::unique_ptr<Actuator> temp_act,
        std::unique_ptr<Actuator> hum_act,
        std::unique_ptr<Actuator> soil_moist_act,
        std::unique_ptr<Actuator> co2_act
    );

    ~ActuatorManager();

    /**
     * \brief Updates parameter values and sets conditional variable
     * 
     * \param frame SENS_FRAME received from Sensor
     */
    void update_parameters(SENS_FRAME frame);

    /**
     * \brief Updates perf and error values for each actuator
     * 
     * \param params Vector of perf and error values received from the Web process
     */
    void update_configs(std::vector<uint16_t>& params);
};

#endif