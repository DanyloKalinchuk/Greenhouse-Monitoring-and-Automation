#include "pwm_line.hpp"

void PWMLine::set_value(std::fstream* file, uint32_t value){
    *file << std::to_string(value);
    file->flush();
}

PWMLine::PWMLine(uint8_t module_num, uint32_t period, uint32_t duty_cycle) : Line(){
    if (module_num > 1){
        throw std::runtime_error("Invalid pwm module");
    }
    
    std::string path = "/sys/class/pwm/pwmchip0/";

    std::fstream chip_export((path + "export"), std::ios::out);
    if (!chip_export.is_open()){
        throw std::runtime_error("Failed to open " + ((path + "export")));
    }
    
    this->set_value(&chip_export, module_num);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    chip_export.close();

    path += "pwm" + std::to_string(module_num) + "/";
    std::fstream period_file = std::fstream((path + "period"), std::ios::out);
    if (!period_file.is_open()){
        throw std::runtime_error("Failed to open " + path + "period");
    }

    this->set_value(&period_file, period);
    period_file.close();

    this->duty_cycle = std::fstream((path + "duty_cycle"), std::ios::out);
    if (!this->duty_cycle.is_open()){

        throw std::runtime_error("Failed to open " + path + "duty_cycle");
    }

    this->set_value(&this->duty_cycle, duty_cycle);

    this->enable = std::fstream((path + "enable"), std::ios::out);
    if (!enable.is_open()){
        this->duty_cycle.close();

        throw std::runtime_error("Failed to open" + path + "enable");
    }

    this->set_value(&this->enable, PWM_DISABLE);
}

PWMLine::~PWMLine(){
    this->set_value(&this->enable, PWM_DISABLE);

    this->duty_cycle.close();
    this->enable.close();
}

void PWMLine::write(uint32_t value){
    this->set_value(&this->enable, PWM_ENABLE);
    this->set_value(&this->duty_cycle, value);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    this->set_value(&this->enable, PWM_DISABLE);
}

bool PWMLine::read(){
    return true;
}