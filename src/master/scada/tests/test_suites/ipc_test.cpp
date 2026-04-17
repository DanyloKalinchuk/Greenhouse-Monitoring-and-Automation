#include <gtest/gtest.h>
#include "../../ipc/ipc.hpp"

#define MSG_DEF (0)

class IPCTest : public IPC {
    std::mutex test_mtx;
    std::atomic<bool> test_conf_ready = false;

    std::atomic<uint16_t> buff = MSG_DEF;
    std::atomic<bool> buff_ready_to_write = true;

    std::vector<uint16_t> test_params;
    std::vector<SENS_FRAME> test_last_records;

    uint16_t test_read(){
        while (this->buff_ready_to_write.load());
        this->buff_ready_to_write.store(true);
        return this->buff.load();
    }

    void test_write(uint16_t msg){
        while (!this->buff_ready_to_write.load());
        this->buff.store(msg);
        this->buff_ready_to_write.store(false);
    }

    void ipc_handling() override {
        while (this->ipc_on.load()){
            std::vector<uint16_t> buff;
            uint16_t msg = this->test_read();

            if (msg == MSG_CONF){
                for (int i = 0; i < 8; i++){
                    buff.push_back(this->test_read());
                }

                this->handle_configuration(buff);
            }else if (msg == MSG_REQ){
                this->handle_data_request();
            }
        }
    }


    void handle_configuration(std::vector<uint16_t> params) override{
        std::lock_guard<std::mutex> test_lock(this->test_mtx);

        this->test_params = {};
        for (const uint16_t& param : params){
            this->test_params.push_back(param);
        }

        this->test_conf_ready.store(true);
    }

    void handle_data_request() override{
        std::lock_guard<std::mutex> test_lock(this->test_mtx);
        uint16_t msg = this->test_last_records.size();
        this->test_write(msg);

        for (SENS_FRAME& frame : this->test_last_records){
            msg = frame.sensor_id;
            this->test_write(msg);

            msg = frame.temperature;
            this->test_write(msg);
            msg = frame.humidity;
            this->test_write(msg);
            msg = frame.soil_moisture;
            this->test_write(msg);
            msg = frame.co2;
            this->test_write(msg);
        }
    }

    public:
    IPCTest() : IPC(0) {
        this->ipc_on.store(true);
        this->ipc_thread = std::thread(&IPC::ipc_handling, this);
    }

    void set_last_records(std::vector<SENS_FRAME> last_records){
        this->test_last_records = last_records;
    }

    std::vector<SENS_FRAME> test_make_request(){
        uint16_t msg = MSG_REQ;
        std::vector<SENS_FRAME> received_data;

        this->test_write(msg);

        uint16_t frame_num = this->test_read();
        for (int i = 0; i < frame_num; i++){
            SENS_FRAME frame;

            frame.sensor_id = this->test_read();
            frame.temperature = this->test_read();
            frame.humidity = this->test_read();
            frame.soil_moisture = this->test_read();
            frame.co2 = this->test_read();

            received_data.push_back(frame);
        }

        return received_data;
    }

    bool test_configure(std::vector<uint16_t> params){
        uint16_t msg = MSG_CONF;

        this->test_write(msg);

        for (const uint16_t& param : params){
            msg = param;
            this->test_write(msg);
        }

        while (!this->test_conf_ready.load());
        std::lock_guard<std::mutex> test_lock(this->test_mtx);
        for (int i = 0; i < CONF_BUFF_SIZE; i++){
            if (this->test_params[i] != params[i]){
                return false;
            }
        }
        this->test_conf_ready.store(false);

        return true;
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

    std::vector<SENS_FRAME> received_records = ipc_test.test_make_request();

    ASSERT_EQ(records.size(), received_records.size());

    for (int i = 0; i < received_records.size(); i++){
        EXPECT_EQ(received_records[i].sensor_id, records[i].sensor_id);
        EXPECT_EQ(received_records[i].temperature, records[i].temperature);
        EXPECT_EQ(received_records[i].humidity, records[i].humidity);
        EXPECT_EQ(received_records[i].soil_moisture, records[i].soil_moisture);
        EXPECT_EQ(received_records[i].co2, records[i].co2);
    }
}

TEST_F(IPCTestFixture, Configuration){
    std::vector<uint16_t> params = {20, 5, 30, 2, 40, 6, 10, 8};

    EXPECT_TRUE(ipc_test.test_configure(params));
}