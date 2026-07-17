#include <gtest/gtest.h>
#include <mutex>
#include <vector>
#include <memory>
#include <chrono>
#include "../../env_control/env_control.hpp"
#include "../../env_control/actuator/actuator.hpp"

#define TEMP_PERF 25
#define TEMP_ERR 10

#define HUM_PERF 40
#define HUM_ERR 10

#define MOIST_PERF 50
#define MOIST_ERR 20

#define CO2_PERF 60
#define CO2_ERR 15

class ActuatorTest : public Actuator{
    std::shared_ptr<int16_t> curr_value;
    std::mutex act_mtx;

    public:
    ActuatorTest(int16_t init_perf, uint8_t init_error, std::shared_ptr<int16_t> init_curr) : 
        Actuator(init_perf, init_error) {
            this->curr_value = std::shared_ptr(init_curr);
        }

    void handle_changes(int16_t param_value) override {
        std::lock_guard<std::mutex> lock_act(this->act_mtx);

        if (*this->curr_value <= this->perf - this->error){
            *this->curr_value += this->error / 2;
        }else if (*this->curr_value >= this->perf + this->error){
            *this->curr_value -= this->error / 2;
        }
    }

    void set_curr_value(int16_t curr_value){
        std::lock_guard<std::mutex> lock_act(this->act_mtx);
        *this->curr_value = curr_value;
    }

    int16_t get_curr_value(){
        std::lock_guard<std::mutex> lock_act(this->act_mtx);
        return *this->curr_value;
    }
};

class EnvControlTest : public EnvControl{
    SENS_FRAME input_frame;
    std::mutex frame_mtx;

    std::shared_ptr<int16_t> temp_curr = std::make_shared<int16_t>(TEMP_PERF);
    std::shared_ptr<int16_t> hum_curr = std::make_shared<int16_t>(HUM_PERF);
    std::shared_ptr<int16_t> moist_curr = std::make_shared<int16_t>(MOIST_PERF);
    std::shared_ptr<int16_t> co2_curr = std::make_shared<int16_t>(CO2_PERF);

    public:
    EnvControlTest() :
        EnvControl(
            std::make_unique<ActuatorTest>(TEMP_PERF, TEMP_ERR, temp_curr), 
            std::make_unique<ActuatorTest>(HUM_PERF, HUM_ERR, hum_curr), 
            std::make_unique<ActuatorTest>(MOIST_PERF, MOIST_ERR, moist_curr),
            std::make_unique<ActuatorTest>(CO2_PERF, CO2_ERR, co2_curr)
        ) {}

    void handle_comm() override {
        while (this->comm_on.load()){
            std::lock_guard<std::mutex> lock_frame(this->frame_mtx);
            SENS_FRAME frame = this->input_frame;

            if (frame.sensor_id != DEFAULT_ID){
                this->actuator_manager->update_parameters(frame);

                this->input_frame.sensor_id = DEFAULT_ID;
            } 
        }
    }

    std::vector<int16_t> get_curr_values(){
        std::vector<int16_t> curr_values(4);

        curr_values[0] = *this->temp_curr;
        curr_values[1] = *this->hum_curr;
        curr_values[2] = *this->moist_curr;
        curr_values[3] = *this->co2_curr;

        return curr_values;
    }

    void set_input_frame(SENS_FRAME frame){
        std::lock_guard<std::mutex> lock_frame(this->frame_mtx);
        this->input_frame = frame;
    }
};

struct EnvControlFixture :  public testing::Test {
    protected:
    EnvControlTest env_control = EnvControlTest();
};

TEST_F(EnvControlFixture, ParamControl){
    SENS_FRAME frame;
    frame.sensor_id = 1;
    frame.temperature = TEMP_PERF - 40;
    frame.humidity = HUM_PERF + 30;
    frame.soil_moisture = MOIST_PERF - (MOIST_ERR - 1);
    frame.co2 = CO2_PERF - CO2_ERR;

    

    this->env_control.set_input_frame(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    std::vector<int16_t> curr_values = this->env_control.get_curr_values();

    EXPECT_EQ(frame.temperature, (curr_values[0] - (TEMP_ERR / 2)));
    EXPECT_EQ(frame.humidity, (curr_values[1] + (HUM_ERR / 2)));
    EXPECT_EQ(frame.soil_moisture, curr_values[2]);
    EXPECT_EQ(frame.co2, (curr_values[3] - (CO2_ERR / 2)));
}