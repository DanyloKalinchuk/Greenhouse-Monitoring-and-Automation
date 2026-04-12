#include <gtest/gtest.h>
#include <mutex>
#include <vector>
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
    int16_t curr_value;
    std::mutex act_mtx;

    public:
    ActuatorTest(int16_t init_perf, uint8_t init_error) : 
        Actuator(init_perf, init_error) {
            this->curr_value = init_perf;
        }

    void handle_changes(int16_t param_value) override {
        std::lock_guard<std::mutex> lock_act(this->act_mtx);

        if (this->curr_value <= this->perf - this->error){
            this->curr_value += this->error / 2;
        }else if (this->curr_value >= this->perf + this->error){
            this->curr_value -= this->error / 2;
        }
    }

    void set_curr_value(int16_t curr_value){
        std::lock_guard<std::mutex> lock_act(this->act_mtx);
        this->curr_value = curr_value;
    }

    int16_t get_curr_value(){
        std::lock_guard<mutex> lock_act(this->act_mtx);
        return this->curr_value;
    }
};

class EnvControlTest : public EnvControl{
    SENS_FRAME input_frame;
    std::mutex frame_mtx;

    public:
    EnvControlTest() :
        EnvControl(
            std::make_unique<ActuatorTest>(TEMP_PERF, TEMP_ERR), 
            std::make_unique<ActuatorTest>(HUM_PERF, HUM_ERR), 
            std::make_unique<ActuatorTest>(MOIST_PERF, MOIST_ERR),
            std::make_unique<ActuatorTest>(CO2_PERF, CO2_ERR)
        ) {}

    void handle_comm() override {
        while (this->comm_on.load()){
            this->frame_mtx.lock();
            SENS_FRAME frame = this->input_frame;
            this->frame_mtx.unlock();

            if (frame.sensor_id != DEFAULT_ID){
                static_cast<ActuatorTest*>(this->temp_act.get())->set_curr_value(frame.temperature);
                static_cast<ActuatorTest*>(this->hum_act.get())->set_curr_value(frame.humidity);
                static_cast<ActuatorTest*>(this->moist_act.get())->set_curr_value(frame.soil_moisture);
                static_cast<ActuatorTest*>(this->co2_act.get())->set_curr_value(frame.co2);

                this->temp_act->handle_changes(1);
                this->hum_act->handle_changes(1);
                this->moist_act->handle_changes(1);
                this->co2_act->handle_changes(1);

                this->frame_mtx.lock();
                this->input_frame.sensor_id = DEFAULT_ID;
                this->frame_mtx.unlock();
            } 
        }
    }

    std::vector<int16_t> get_curr_values(){
        std::vector<int16_t> curr_values(4);

        curr_values[0] = static_cast<ActuatorTest*>(this->temp_act.get())->get_curr_value();
        curr_values[1] = static_cast<ActuatorTest*>(this->hum_act.get())->get_curr_value();
        curr_values[2] = static_cast<ActuatorTest*>(this->moist_act.get())->get_curr_value();
        curr_values[3] = static_cast<ActuatorTest*>(this->co2_act.get())->get_curr_value();

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