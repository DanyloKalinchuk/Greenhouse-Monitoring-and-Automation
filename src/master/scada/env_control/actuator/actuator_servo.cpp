#include "actuator_servo.hpp"

ActuatorServo::ActuatorServo(std::unique_ptr<Line> line, int16_t init_perf, uint8_t init_error) :
    Actuator(std::move(line), init_perf, init_error) {}

void ActuatorServo::handle_changes(int16_t param_value){
    std::string message = "Parameter received. Servo state: ";
    uint32_t pos = 0;

    if (param_value > (this->perf + this->error)){
        pos = POS_OPENED;
        message += "OPENED";
    }else if (param_value < (this->perf - this->error)){
        pos = POS_CLOSED;
        message += "CLOSED";
    }else{
        message += "Unchanged";
    }

    if (pos){
        this->line->write(pos);
    }

    Logger::get_instance()->log_out(message, MsgType::INFO);
}