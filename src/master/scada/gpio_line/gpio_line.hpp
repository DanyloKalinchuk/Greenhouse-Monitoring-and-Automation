#ifndef GPIO_LINE_HPP
#define GPIO_LINE_HPP

#include <gpiod.h>
#include <cstdint>
#include <stdexcept>

class GPIOLine {
    struct gpiod_chip *chip;
	struct gpiod_line_request *request;

    uint8_t pin;
    bool input;

    public:
    GPIOLine(uint8_t pin, bool input);
    ~GPIOLine();

    void write(bool level);
    bool read();
    void wait_for_edge_event();
};

#endif