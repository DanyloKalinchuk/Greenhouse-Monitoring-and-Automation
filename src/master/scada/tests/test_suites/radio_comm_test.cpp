#include <gtest/gtest.h>
#include "../../radio_comm/radio_comm.hpp"

class RadioCommTest : public Radio{
    public:
    RadioCommTest() : Radio((uint8_t)(1)) {
        this->reg_sensors = {};
        this->next_sens_id = 1;

        this->update_data_on_disk();
    }

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

    void call_read_data(){
        this->read_data_on_disk();
    }

    void call_update_data(){
        this->update_data_on_disk();
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

    ASSERT_EQ(sensors.count(sens1_expected_inner_id), 1);
    EXPECT_EQ(sensors.at(sens1_expected_inner_id), sens1_id);

    ASSERT_EQ(sensors.count(sens2_expected_inner_id), 1);
    EXPECT_EQ(sensors.at(sens2_expected_inner_id), sens2_id);
}

TEST_F(RadioCommFixture, DataHandlingFromUnregisteredSensor){
    uint32_t sensor_data[SENSOR_DATA_SIZE];
    SENS_FRAME frame;

    sensor_data[0] = 25;
    sensor_data[1] = 30;
    sensor_data[2] = 31;
    sensor_data[3] = 32;
    sensor_data[4] = 33;


    radio.call_sensor_handle_data(sensor_data, &frame);

    EXPECT_EQ(frame.sensor_id, DEFAULT_ID);
    EXPECT_EQ(frame.humidity, sensor_data[1]);
    EXPECT_EQ(frame.temperature, sensor_data[2]);
    EXPECT_EQ(frame.co2, sensor_data[3]);
    EXPECT_EQ(frame.soil_moisture, sensor_data[4]);
}

TEST_F(RadioCommFixture, DataHandlingFromRegisteredSensor){
    uint32_t sensor_data[SENSOR_DATA_SIZE];
    SENS_FRAME frame;
    uint8_t expected_inner_id = 1;

    sensor_data[0] = 25;
    sensor_data[1] = 30;
    sensor_data[2] = 31;
    sensor_data[3] = 32;
    sensor_data[4] = 33;

    radio.call_sensor_init(sensor_data[0]);
    radio.call_sensor_handle_data(sensor_data, &frame);

    EXPECT_EQ(sensor_data[0], radio.get_sensors().at(expected_inner_id));
    EXPECT_EQ(frame.sensor_id, expected_inner_id);
    EXPECT_EQ(frame.humidity, sensor_data[1]);
    EXPECT_EQ(frame.temperature, sensor_data[2]);
    EXPECT_EQ(frame.co2, sensor_data[3]);
    EXPECT_EQ(frame.soil_moisture, sensor_data[4]);
}

TEST_F(RadioCommFixture, ReadUpdateSaves){
    std::map<uint8_t, uint8_t> ids_init;
    std::map<uint8_t, uint8_t> ids_read;

    radio.call_sensor_init(8);
    radio.call_sensor_init(16);
    radio.call_sensor_init(32);

    ids_init = radio.get_sensors();

    radio.call_update_data();
    radio.call_read_data();

    ids_read = radio.get_sensors();

    ASSERT_NO_THROW(ids_read.at(1));
    ASSERT_NO_THROW(ids_init.at(1));
    EXPECT_EQ(ids_read.at(1), ids_init.at(1));

    ASSERT_NO_THROW(ids_read.at(2));
    ASSERT_NO_THROW(ids_init.at(2));
    EXPECT_EQ(ids_read.at(2), ids_init.at(2));

    ASSERT_NO_THROW(ids_read.at(3));
    ASSERT_NO_THROW(ids_init.at(3));
    EXPECT_EQ(ids_read.at(3), ids_init.at(3));
}

