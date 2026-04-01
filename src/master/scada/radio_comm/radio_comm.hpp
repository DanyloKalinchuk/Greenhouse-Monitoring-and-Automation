#ifndef RADIO_COMM
#define RADIO_COMM

#include <RF24/RF24.h>
#include <linux/printk.h>
#include <cstdint>
#include <map>

#define SAVE_PATH ("reg_sensors.bin")

#define MASTER_ID (1)
#define INIT_PIPE (1)
#define DATA_PIPE (2)

#define CE 23
#define CS 19

struct SENS_FRAME{
    uint8_t sensor_id;
    uint16_t humidity;
    uint16_t temperature;
    uint16_t co2;
    uint16_t soil_moisture;
};

class Radio{
    RF24 radio;

    static std::map<uint8_t, uint8_t> reg_sensors;
    static uint8_t next_sens_id;

    private:
    void check_data_on_disk();

    public:
    Radio();

    SENS_FRAME read_data();
};

#endif