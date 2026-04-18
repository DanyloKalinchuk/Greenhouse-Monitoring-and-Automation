#include <gtest/gtest.h>
#include <queue>
#include "../../ipc/ipc.hpp"

#define BUFF_EMPTY 0

class IPCTest : public IPC {
    std::queue<uint16_t> buff;
    
    void ipc_write(uint16_t msg) override {
        this->buff.push(msg);
    }

    uint16_t ipc_read() override {
        uint16_t msg = BUFF_EMPTY;

        if (!this->buff.empty()){
            msg = this->buff.front();
            this->buff.pop();
        }
        return msg;
    }
    
    public:
    IPCTest() : IPC(0) {}

    std::vector<SENS_FRAME> make_request(){
        this->ipc_write(MSG_REQ);
        std::vector<SENS_FRAME> frames;

        this->handle_msg();
        
        uint16_t frame_num = this->ipc_read();
        for (int i = 0; i < frame_num; i++){
            SENS_FRAME frame;
            frame.sensor_id = this->ipc_read();
            frame.temperature = this->ipc_read();
            frame.humidity = this->ipc_read();
            frame.soil_moisture = this->ipc_read();
            frame.co2 = this->ipc_read();

            frames.push_back(frame);
        }

        return frames;
    }
};

struct IPCTestFixture : public testing::Test {
    protected:
    IPCTest ipc_test = IPCTest();

    struct SENS_FRAME frame1 = {
        1,
        40,
        20,
        50,
        30
    };

    struct SENS_FRAME frame2 = {
        2,
        30,
        70,
        10,
        56
    };
};

TEST_F(IPCTestFixture, DataRequest){
    std::vector<SENS_FRAME> records = {frame1, frame2};
    ipc_test.set_last_records(records);

    std::vector<SENS_FRAME> received_records = ipc_test.make_request();

    ASSERT_EQ(records.size(), received_records.size());

    for (int i = 0; i < received_records.size(); i++){
        EXPECT_EQ(received_records[i].sensor_id, records[i].sensor_id);
        EXPECT_EQ(received_records[i].temperature, records[i].temperature);
        EXPECT_EQ(received_records[i].humidity, records[i].humidity);
        EXPECT_EQ(received_records[i].soil_moisture, records[i].soil_moisture);
        EXPECT_EQ(received_records[i].co2, records[i].co2);
    }
}
