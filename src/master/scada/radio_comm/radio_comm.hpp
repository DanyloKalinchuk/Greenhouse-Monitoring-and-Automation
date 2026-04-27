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
#define SAVE_PATH ("reg_sensors.bin")
#endif

#ifndef RADIO_LOGS_PATH
#define RADIO_LOGS_PATH ("radio_logs.log")
#endif

#define DEFAULT_ID (0)
#define MASTER_ID (1)
#define INIT_ADDRESS ("init_address")
#define INIT_PIPE (0)
#define DATA_PIPE (1)
#define SENSOR_DATA_SIZE (5)

#define CE 22
#define CS 0
#define IRQ 25

struct SENS_FRAME{
    uint8_t sensor_id;
    uint32_t humidity;
    uint32_t temperature;
    uint32_t co2;
    uint32_t soil_moisture;
};

class Radio{
    RF24 radio;
    RadioLogs radio_logs = RadioLogs(RADIO_LOGS_PATH);
    std::unique_ptr<GPIOLine> irq_line = nullptr;

    protected:
    std::map<uint8_t, uint8_t> reg_sensors;
    uint8_t next_sens_id = 1;

    void read_data_on_disk();
    void update_data_on_disk();

    void sensor_init(uint8_t senor_id);
    void sensor_handle_data(uint32_t sensor_data[SENSOR_DATA_SIZE], SENS_FRAME* sens_frame);

    public:
    Radio();
    Radio(uint8_t dummy);
    ~Radio();

    virtual SENS_FRAME handle_communications();
};

#endif