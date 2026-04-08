#define SAVE_PATH ("test_sensors.bin")
#define RADIO_LOGS_PATH ("test_logs.txt")

#include <gtest/gtest.h>
#include "../../radio_comm/radio_comm.hpp"

class RadioCommTest : public Radio{
    public:
    RadioCommTest() : Radio((uint8_t)(1)) {}

    std::map<uint8_t, uint8_t> get_sensors(){
        return this->reg_sensors;
    }

    void set_sensors(std::map<uint8_t, uint8_t> reg_sensors){
        this->reg_sensors = reg_sensors;
    }

    SENS_FRAME handle_communications() override{
        SENS_FRAME frame;
        frame.sensor_id = DEFAULT_ID;
        return frame;
    }

    void call_sensor_init(uint8_t sensor_id){
        this->sensor_init(sensor_id);
    }

    void call_sensor_handle_data(uint32_t sensor_data[SENSOR_DATA_SIZE], SENS_FRAME* sens_frame){
        this->sensor_handle_data(sensor_data, sens_frame);
    }
};

struct RadioCommFixture : public testing::Test {
    protected:
    RadioCommTest radio = RadioCommTest();
};

TEST_F(RadioCommFixture, SensorRegistration){
    const uint8_t sens1_expected_inner_id= 1;
    const uint8_t sens1_id= 200;

    const uint8_t sens2_expected_inner_id = 2;
    const uint8_t sens2_id = 10;

    radio.call_sensor_init(sens1_id);
    radio.call_sensor_init(sens2_id);
    radio.call_sensor_init(sens1_id);

    std::map<uint8_t, uint8_t> sensors = radio.get_sensors();

    ASSERT_EQ(sensors.size(), 2);

    ASSERT_EQ(sensors.count(sens1_id), 1);
    EXPECT_EQ(sensors.at(sens1_id), sens1_expected_inner_id);

    ASSERT_EQ(sensors.count(sens2_id), 1);
    EXPECT_EQ(sensors.at(sens2_id), sens2_expected_inner_id);
}

