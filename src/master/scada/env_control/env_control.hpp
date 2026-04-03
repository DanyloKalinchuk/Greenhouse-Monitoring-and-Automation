#ifndef ENV_CONTROL
#define ENV_CONTROL

#include "../radio_comm/radio_comm.hpp"
#include <map>

class EnvControl{
    SENS_FRAME last_received;
    std::map<uint8_t, SENS_FRAME> last_records;

    public:
    EnvControl();
    void handle_data(SENS_FRAME frame);
};

#endif