#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

#include "../../gpio_line/gpio_line.hpp"
#include <string>
#include <stdexcept>
#include <cstdint>
#include <memory>

#define gpiod_val(val) ((val) == 0 ? GPIOD_LINE_VALUE_INACTIVE : GPIOD_LINE_VALUE_ACTIVE)

class Actuator {
    std::unique_ptr<GPIOLine> pin = nullptr;
    const bool line_initialized;

    protected:
    int16_t perf;
    uint8_t error;

    public:
    Actuator(uint8_t pin, int16_t init_perf, uint8_t init_error);
    Actuator(int16_t init_perf, uint8_t init_error);

    virtual void handle_changes(int16_t param_value);
    void set_target(int16_t perf, uint8_t error);
};

#endif