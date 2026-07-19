#ifndef GPIO_LINE_HPP
#define GPIO_LINE_HPP

#include "line.hpp"
#include <gpiod.h>
#include <cstdint>
#include <stdexcept>
#include <sys/poll.h>

#define EDGE_WAIT_TIMEOUT 500

#define PIN_HIGH 1
#define PIN_LOW 0

/// \brief Class for managing GPIO usage 
class GPIOLine : public Line {
    struct gpiod_chip *chip;
	struct gpiod_line_request *request;
    struct pollfd pfd;

    uint8_t pin; ///< GPIO pin handled by the class instance
    bool input; ///< Is True if the GPIO configured to the input mode, False otherwise

    protected:
    void set_pin(uin8_t value);

    public:
    /**
     * \brief Constructs GPIOLine class and acquires gpio resources
     * 
     * \param pin The GPIO pin represented by the class instance
     * \param input Boolean variable. Set True to initialize GPIO as input, False otherwise
     */
    GPIOLine(uint8_t pin, bool input);
    ~GPIOLine();

    /**
     * \brief Set GPIO pin logic level
     * \attention Only applicable for output mode
     * 
     * \param value Represents Low logic level if 0, High otherwise
     */
    void write(uint32_t value) override;

    /// \brief Return logic level of the GPIO pin
    bool read();

    /**
     * \brief Blocks until a falling edge event is detected on the GPIO pin
     * 
     * \return True if edge event occurred, False if timeout event occured
     */
    bool wait_for_edge_event();
};

#endif