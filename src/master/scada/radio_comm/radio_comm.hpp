#ifndef RADIO_COMM
#define RADIO_COMM

#include "../gpio_line/gpio_line.hpp"
#include <RF24/RF24.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>

#include "radio_logs/radio_logs.hpp"

#ifndef SAVE_PATH
#define SAVE_PATH ("reg_sensors.bin") ///< File for storing registered sensors
#endif

#ifndef RADIO_LOGS_PATH
#define RADIO_LOGS_PATH ("radio_logs.log") ///< File for radio communication logs
#endif

#define DEFAULT_ID (0) ///< Default ID value for Sensors
#define MASTER_ID (1) ///< Master ID value
#define INIT_ADDRESS ("init_address") ///< Address for Sensor initialization. Must be identical to the init_address in \ref sensor.ino
#define INIT_PIPE (0) ///< Pipe number for the Sensor initialization
#define DATA_PIPE (1) ///< Pipe number for Sensor data receive
#define SENSOR_DATA_SIZE (5) ///< Number of 32-bit values received from Sensor

#define CE 22 ///< CE line of the nRF24L01 module
#define CS 0 ///< CS line of the nRF24L01 module
#define IRQ 25 ///< IRQ line of the nRF24L01 module

/// \brief Contains data received from Sensor
struct SENS_FRAME{
    uint8_t sensor_id; ///< Inner ID value
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
    RF24 radio;
    RadioLogs radio_logs = RadioLogs(RADIO_LOGS_PATH);
    std::unique_ptr<GPIOLine> irq_line = nullptr;

    /**
     * \brief Registers Sensor
     * 
     * \param sensor_id Sensor ID received from Sensor
     * \return inner_id that represents sensor within the Master device and database
     */
    uint8_t sensor_register(uint8_t sensor_id);

    protected:
    std::map<uint8_t, uint8_t> reg_sensors; ///< Sensor IDs and inner IDs of sensors read from the SAVE_PATH file
    uint8_t next_sens_id = 1; ///< Next free inner ID

    void read_data_on_disk(); ///< Reads reg_sensors and next_sens_id values from SAVE_PATH file
    void update_data_on_disk(); ///< Rewrites SAVE_PATH file with reg_sensors and next_sens_id values
    void add_sensor_on_disk(); ///< Adds last register to the SAVE_PATH file and increments next_sens_id value in it

    /**
     * \brief Initializes Sensor
     * Assigns inner ID to the provided Sensor ID if none is assigned
     * 
     * \param sensors_id ID of the initializing Sensor
     */
    void sensor_init(uint8_t sensor_id);

    /**
     * \brief Packs sensor_data array into a SENS_FRAME structure
     * 
     * \param[in] sensor_data Data received from a Sensor
     * \param[out] sens_frame Pointer to a SENS_FRAME structure
     */
    void sensor_handle_data(uint32_t sensor_data[SENSOR_DATA_SIZE], SENS_FRAME* sens_frame);

    public:
    Radio();
    Radio(uint8_t dummy); ///< Constructor used in the testing. Does not initialize hardware resources
    ~Radio();

    /**
     * \brief Handles radio communication
     * Initializes Sensor or receives SENS_FRAMEs depending on the pipe
     */
    virtual SENS_FRAME handle_communications();
};

#endif