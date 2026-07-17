#ifndef PWM_LINE_HPP
#define PWM_LINE_HPP

#include "line.hpp"
#include <fstream>
#include <cstdint>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <stdexcept>

#define PWM_ENABLE 1
#define PWM_DISABLE 0

#define PWM0 0 ///< PWM channel 0
#define PWM1 1 ///< PWM channel 1

/// \brief Class for PWM usage via sysfs
class PWMLine : public Line{
    std::fstream duty_cycle;
    std::fstream enable;

    protected:
    /**
     * \brief Echoes value to a corresponding file
     * 
     * \param file Pointer to a file inside a sysfs
     * \param value Value to be echoed
     */
    void set_value(std::fstream* file, uint32_t value);

    public:
    /**
     * \brief PWMLine constrictor
     * 
     * \param channel_num Number of a PWM channel 
     * \note In the case of Raspberry Pi 4 there are only 2 channels available: 0 and 1
     * 
     * \param period Period value
     * \param duty_cycle Initial duty cycle value
     */
    PWMLine(uint8_t channel_num, uint32_t period, uint32_t duty_cycle);
    ~PWMLine();

    /**
     * \brief Sets new duty cycle value
     * 
     * \param value Duty cycle value
     */
    void write(uint32_t value) override;

    /// \brief Unused function. Always returns true
    bool read() override;
};

#endif