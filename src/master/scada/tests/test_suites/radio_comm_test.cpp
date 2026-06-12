#include <gtest/gtest.h>
#include "../../radio_comm/radio_comm.hpp"

class RadioCommTest : public Radio{
    public:
    SENS_FRAME handle_communications() override{
        SENS_FRAME frame;
        frame.sensor_id = DEFAULT_ID;
        return frame;
    }
};

struct RadioCommFixture : public testing::Test {
    protected:
    RadioCommTest radio = RadioCommTest();
};
