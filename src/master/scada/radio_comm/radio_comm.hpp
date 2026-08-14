#ifndef RADIO_COMM
#define RADIO_COMM

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>

#include "../logger/logger.hpp"

#define RADIO_LOGS_PATH ("radio_logs.log") ///< File for radio communication logs

#define DEFAULT_ID (0) ///< Default ID value for Sensors
#define MASTER_ID (1) ///< Master ID value
#define SENSOR_DATA_SIZE (5) ///< Number of 32-bit values received from Sensor

/// \brief Contains data received from Sensor
struct SENS_FRAME{
    uint8_t sensor_id;
    uint8_t humidity;
    int8_t temperature;
    uint8_t co2;
    uint8_t soil_moisture;
};

/**
 * \brief Class for managing radio communication from the Master side
 * Contains logic for Sensor registration/initialization and logging radio communication events
 */
class Radio{
    public:
    /**
     * \brief Handles radio communication
     * Initializes Sensor or receives SENS_FRAMEs
     */
    virtual SENS_FRAME handle_communications() = 0;
};

#endif