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

#define PWM0 0
#define PWM1 1

class PWMLine : public Line{
    std::fstream duty_cycle;
    std::fstream enable;

    protected:
    void set_value(std::fstream* file, uint32_t value);

    public:
    PWMLine(uint8_t module_num, uint32_t period, uint32_t duty_cycle);
    ~PWMLine();

    void write(uint32_t value) override;
    bool read() override;
};

#endif