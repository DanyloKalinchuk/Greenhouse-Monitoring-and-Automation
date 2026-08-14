#ifndef SENSORS_H
#define SENSORS_H

#include <zephyr/logging/log.h>
#include <zephyr/dsp/types.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/regulator.h>

struct SENS_DATA {
    uint8_t sensor_id;
    int8_t temperature;
    uint8_t humidity;
    uint8_t co2;
    uint8_t soil_moisture;
};

uint8_t sensors_init(uint16_t periph_prep_s);
uint8_t sensors_read(struct SENS_DATA *data);

#endif