#ifndef GPIO_LINE_HPP
#define GPIO_LINE_HPP

#include <gpiod.h>
#include <cstdint>
#include <stdexcept>

/// \brief Class for managing GPIO usage 
class GPIOLine {
    struct gpiod_chip *chip;
	struct gpiod_line_request *request;

    uint8_t pin; ///< GPIO pin handled by the class instance
    bool input; ///< Is True if the GPIO configured to the input mode, False otherwise

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
     * \param level Represents High logic level if True, Low otherwise
     */
    void write(bool level);

    /// \brief Return logic level of the GPIO pin
    bool read();

    /// \brief Blocks until a falling edge event is detected on the GPIO pin
    void wait_for_edge_event();
};

#endif