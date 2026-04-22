#ifndef GPIO_LINE_HPP
#define GPIO_LINE_HPP

#include <gpiod.h>
#include <cstdint>
#include <stdexcept>

class GPIOLine {
    struct gpiod_chip *chip;
	struct gpiod_line_request *request;

    uint8_t pin;

    public:
    GPIO_LINE(uint8_t pin);
    ~GPIO_LINE();

    void write(bool level);
    bool read();
};

#endif