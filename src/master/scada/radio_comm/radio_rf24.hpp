#ifndef RADIO_RF24
#define RADIO_RF24

#include <RF24/RF24.h>
#include "radio_comm.hpp"
#include "../line/gpio_line.hpp"

#define INIT_ADDRESS ("init_address") ///< Address for Sensor initialization. Must be identical to the init_address in \ref sensor.ino
#define INIT_PIPE (0) ///< Pipe number for the Sensor initialization
#define DATA_PIPE (1) ///< Pipe number for Sensor data receive

#define CE 22 ///< CE line of the nRF24L01 module
#define CS 0 ///< CS line of the nRF24L01 module
#define IRQ 25 ///< IRQ line of the nRF24L01 module

class Radio_RF24 : public Radio{
    RF24 radio;
    std::unique_ptr<GPIOLine> irq_line;

    protected:
     /**
     * \brief Packs sensor_data array into a SENS_FRAME structure
     * 
     * \param[in] sensor_data Data received from a Sensor
     * \param[out] sens_frame Pointer to a SENS_FRAME structure
     */
    void sensor_handle_data(uint32_t sensor_data[SENSOR_DATA_SIZE], SENS_FRAME* sens_frame) override;

    public:
    Radio_RF24();

    /**
     * \brief Handles radio communication
     * Initializes Sensor or receives SENS_FRAMEs
     */
    SENS_FRAME handle_communications() override;
};

#endif