#ifndef PWM_MANUAL_LINE_HPP
#define PWM_MANUAL_LINE_HPP

#include "gpio_line.hpp"
#include <cstdint>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>

/// \brief Class for using PWM via GPIOLine
class PWMManualLine : public GPIOLine{
    uint32_t period;
    std::atomic<uint32_t> duty_cycle;
    std::atomic<bool> enable;

    std::thread sim_pwm_thread; ///< PWM main loop
    std::atomic<bool> sim_pwm_on; ///< Keeps main loop alive
    std::mutex sim_pwm_mtx; ///< Used for sim_pwm_cv wait
    std::condition_variable sim_pwm_cv; ///< Conditional variable for PWM main loop

    protected:
    /// \brief PWM main loop routine 
    void sim_pwm_handler();

    void set_duty_cycle(uint32_t duty_cycle);
    void set_eanble(bool enable);

    public:
    PWMManualLine(uint8_t pin, uint32_t period, uint32_t duty_cycle);
    ~PWMManualLine();

    void write(uint32_t value) override;
};

#endif