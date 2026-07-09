#ifndef RADIO_COMM
#define RADIO_COMM

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>

#include "../logger/logger.hpp"

#define RADIO_LOGS_PATH ("logs/radio_logs.log") ///< File for radio communication logs

#define DEFAULT_ID (0) ///< Default ID value for Sensors
#define MASTER_ID (1) ///< Master ID value
#define SENSOR_DATA_SIZE (5) ///< Number of 32-bit values received from Sensor

/// \brief Contains data received from Sensor
struct SENS_FRAME{
    uint8_t sensor_id;
    uint32_t humidity;
    uint32_t temperature;
    uint32_t co2;
    uint32_t soil_moisture;
};

/**
 * \brief Class for managing radio communication from the Master side
 * Contains logic for Sensor registration/initialization and logging radio communication events
 */
class Radio{
    protected:
    /**
     * \brief Packs sensor_data array into a SENS_FRAME structure
     * 
     * \param[in] sensor_data Data received from a Sensor
     * \param[out] sens_frame Pointer to a SENS_FRAME structure
     */
    virtual void sensor_handle_data(uint32_t sensor_data[SENSOR_DATA_SIZE], SENS_FRAME* sens_frame) = 0;

    public:
    /**
     * \brief Handles radio communication
     * Initializes Sensor or receives SENS_FRAMEs
     */
    virtual SENS_FRAME handle_communications() = 0;
};

#endif