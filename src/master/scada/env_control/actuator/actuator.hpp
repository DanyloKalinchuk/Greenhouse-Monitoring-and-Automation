#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

#include "../../line/line.hpp"
#include "../../logger/logger.hpp"
#include <string>
#include <stdexcept>
#include <cstdint>
#include <memory>

/**
 * \brief Class for managing actuators for environmental parameters control
 */
class Actuator {
    protected:
    std::unique_ptr<Line> line = nullptr;
    const bool line_initialized; ///< True if the GPIO pin was acquired, False for hardware-less instances

    int16_t perf; ///< The perfect value of the environment parameter
    uint8_t error; ///< The difference between the upper and lower limit of the parameter and the corresponding perf value.

    public:
    /**
     * \brief Main constructor for the Actuator class
     * 
     * \param line Line object representing a GPIO pin
     * \param init_perf Initial perfect value of the environment parameter
     * \param init_error Initial difference between the upper and lower limit of the parameter and the corresponding perf value.
     */
    Actuator(std::unique_ptr<Line> line, int16_t init_perf, uint8_t init_error);

    /**
     * \brief Hardware-less Actuator constructor
     * 
     * \param init_perf Initial perfect value of the environment parameter
     * \param init_error Initial difference between the upper and lower limit of the parameter and the corresponding perf value.
     */
    Actuator(int16_t init_perf, uint8_t init_error);

    /**
     * \brief Handle environment control logic accordingly to the provided value
     * 
     * \param param_value Current value of the controlled parameter
     * \warning Must be overridden in a derived class
     */
    virtual void handle_changes(int16_t param_value);
    
    /**
     * \brief Updates Actuator's perf and error values
     * 
     * \param perf New perf value
     * \param error New error value
     */
    void set_target(int16_t perf, uint8_t error);
};

#endif