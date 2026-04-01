#ifndef ENV_CONTROL
#define ENV_CONTROL

#include "../radio_comm/radio_comm.hpp"

class EnvControl{
    static SENS_FRAME last_received;

    public:
    EnvControl();
    void handle_data(SENS_FRAME frame);
};

#endif