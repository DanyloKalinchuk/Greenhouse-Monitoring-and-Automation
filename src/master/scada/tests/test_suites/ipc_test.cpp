#include <gtest/gtest.h>
#include "../../ipc/ipc.hpp"

class IPCTest : public IPC {
    std::mutex test_mtx;
    std::atomic<bool> test_conf_ready = false;

    int test_sfd;
    struct sockaddr_un test_addr;

    std::vector<uint16_t> test_params(CONF_BUFF_SIZE);
    std::vector<SENS_FRAME> test_last_records;

    void test_write(uint16_t* msg){
        if (write(this->test_sfd, msg, sizeof(*msg)) < sizeof(*msg)){
            throw std::runtime_error("Failed to SEND IPC TEST message");
        }
    }

    uint16_t test_read(){
        uint16_t msg;
        if (read(this->test_sfd, &msg, sizeof(msg)) < sizeof(msg)){
            throw std::runtime_error("Failed to READ IPC TEST message");
        }

        return msg;
    }

    void handle_configuration(std::vector<uint16_t> params) override{
        std::lock_guard<std::mutex> test_lock(this->test_mtx);
        for (int i = 0; i < CONF_BUFF_SIZE; i++){
            this->test_params[i] = params[i];
        }

        this->test_conf_ready.store(true);
    }

    void handle_data_request() override{
        std::lock_guard<std::mutex> test_lock(this->test_mtx);
        uint16_t msg = this->test_last_records.size();
        this->ipc_write(&msg);

        for (SENS_FRAME& frame : this->test_last_records){
            msg = frame.sensor_id;
            this->ipc_write(&msg);

            msg = frame.temperature;
            this->ipc_write(&msg);
            msg = frame.humidity;
            this->ipc_write(&msg);
            msg = frame.soil_moisture;
            this->ipc_write(&msg);
            msg = frame.co2;
            this->ipc_write(&msg);
        }
    }

    public:
    IPCTest() : IPC() {
        this->test_sfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sfd == -1){
            throw std::runtime_error("Failed to open test_sfd");
        }

        memset(&this->test_addr, 0, sizeof(struct sockaddr_un));
        this->test_addr.sun_family = AF_UNIX;
        strcpy(this->test_addr.sun_path, SOCKET_PATH);

        if (connect(sfd, (struct sockaddr *) &this->test_addr, sizeof(struct sockaddr_un)) == -1){
            throw std::runtime_error("Failed to connect TEST");
        }
    }

    void set_last_records(std::vector<SENS_FRAME> last_records){
        this->test_last_records = last_records;
    }

    std::vector<SENS_FRAME> test_make_request(){
        uint32_t msg = MSG_REQ;
        std::vector<SENS_FRAME> received_data;

        this->test_write(&msg);

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
        uint32_t msg = MSG_CONF;

        this->test_write(&msg);

        for (const uint16_t& param : params){
            msg = param;
            this->test_write(&msg);
        }

        while (!this->test_conf_ready.load())
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