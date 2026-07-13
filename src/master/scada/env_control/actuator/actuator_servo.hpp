#ifndef ACTUATORSERVO_HPP
#define ACTUATORSERVO_HPP

#include "actuator.hpp"
#include <thread>

#define PERIOD 20000000

#define POS_LEFT 1000000
#define POS_MID 1500000
#define POS_RIGHT 2000000

#define POS_CLOSED POS_LEFT
#define POS_OPENED POS_RIGHT

#define POS_INIT POS_CLOSED
#define POS_OFF POS_CLOSED

class ActuatorServo : public Actuator{
    public:
    ActuatorServo(std::unique_ptr<Line> line, int16_t init_perf, uint8_t init_error);

    void handle_changes(int16_t param_value) override;
};

#endif