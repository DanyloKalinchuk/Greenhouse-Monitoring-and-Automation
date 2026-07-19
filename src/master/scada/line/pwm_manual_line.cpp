#include "pwm_manual_line.hpp"

void PWMManualLine::sim_pwm_handler(){
    while(this->sim_pwm_on.load()){
        if (!this->enable.load()){
            std::unique_lock<std::mutex> sim_pwm_lock(this->sim_pwm_mtx);
            this->sim_pwm_cv.wait(sim_pwm_lock, [this]{return this->enable.load();});
        }

        this->set_pin(PIN_HIGH);
        std::this_thread::sleep_for(std::chrono::microseconds(this->duty_cycle.load()));

        this->set_pin(PIN_LOW);
        std::this_thread::sleep_for(std::chrono::microseconds(this->period - this->duty_cycle.load()));
    }
}

void PWMManualLine::set_duty_cycle(uint32_t duty_cycle){
    this->duty_cycle.store(duty_cycle);
}

void PWMManualLine::set_eanble(bool enable){
    this->enable.store(enable);
    
    if (this->enable.load()){
        this->sim_pwm_cv.notify_all();
    }
}

PWMManualLine::PWMManualLine(uint8_t pin, uint32_t period, uint32_t duty_cycle) :
    GPIOLine(pin, false)
{
    this->period = period;
    this->duty_cycle.store(duty_cycle);

    this->sim_pwm_on.store(true);
    this->sim_pwm_thread = std::thread(&PWMManualLine::sim_pwm_handler, this);
}

PWMManualLine::~PWMManualLine(){
    this->sim_pwm_on.store(false);

    this->set_eanble(true);
    this->sim_pwm_cv.notify_all();

    if (this->sim_pwm_thread.joinable()){
        this->sim_pwm_thread.join();
    }
}

void PWMManualLine::write(uint32_t value){
    this->set_duty_cycle(value);
    this->set_eanble(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    this->set_eanble(false);
}
