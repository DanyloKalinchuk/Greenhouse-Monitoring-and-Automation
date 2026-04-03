#ifndef RADIO_COMM
#define RADIO_COMM

#include <RF24/RF24.h>
#include <linux/printk.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <map>
#include <stdexcept>

#include "../radio_logs/radio_logs.hpp"

#define SAVE_PATH ("reg_sensors.bin")
#define RADIO_LOGS_PATH("radio_logs.txt")

#define DEFAULT_ID (0)
#define MASTER_ID (1)
#define INIT_ADDRESS ("init_address")
#define INIT_PIPE (1)
#define DATA_PIPE (2)

#define CE 22
#define CS 0

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

    std::map<uint8_t, uint8_t> reg_sensors;
    uint8_t next_sens_id = 1;

    private:
    void read_data_on_disk();
    void update_data_on_disk();
    void clean_data_on_disk();

    public:
    Radio();

    SENS_FRAME handle_communications();
};

#endif