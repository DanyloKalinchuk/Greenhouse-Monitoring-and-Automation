#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

#include <gpiod.h>
#include <string>
#include <stdexcept>
#include <cstdint>

#define gpiod_val(val) ((val) == 0 ? GPIOD_LINE_VALUE_INACTIVE : GPIOD_LINE_VALUE_ACTIVE)

class Actuator {
    struct gpiod_chip *chip;
	struct gpiod_line_request *request;

    uint8_t pin;
    int16_t perf;
    uint8_t error;

    public:
    Actuator(uint8_t pin, int16_t init_perf, uint8_t init_error);
    ~Actuator();

    virtual void handle_changes(itn16_t param_value);
    void set_target(int16_t perf, uint8_t error);
};

#endif