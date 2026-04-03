#include "radio_comm.hpp"

Radio::Radio(){
    this->radio_logs.log_out(MASTER_ID, MasterStart);
    this->radio = RF24(CE, CS);

    if (!this->radio.begin()){
        this->radio_logs.log_out(MASTER_ID, MasterFail);
        throw new std::runtime_error("Failed to initialize the radio module");
    }
    pr_info("Radio module initialized");

    this->read_data_on_disk();

    this->radio.setPALevel(RF24_PA_LOW);
    this->radio.setDataRate(RF24_250KBPS);
    this->radio.setRetries(10, 15);

    this->radio.startListening();
    this->radio.openReadPipe(INIT_PIPE, (uint8_t*)(INIT_ADDRESS));
    this->radio.openReadPipe(DATA_PIPE, (uint64_t)(MASTER_ID));
}

SENS_FRAME Radio::handle_communications(){
    uint8_t curr_pipe;
    SENS_FRAME sens_frame;

    while (!this->radio.available(&curr_pipe));

    if (curr_pipe == INIT_PIPE){
        const uint8_t master_id = MASTER_ID;
        uint8_t sensor_id;
        this->radio.read(&sensor_id, sizeof(sensor_id));

        bool is_registered = false;
        uint8_t inner_id;

        for (const auto [key, id] : this->reg_sensors){
            if (key == sensor_id){
                is_registered = true;
                inner_id = id;
                break;
            }
        }

        if (!is_registered){
            this->reg_sensors[sensor_id] = this->next_sens_id;
            inner_id = this->next_sens_id;
            this->next_sens_id++;
        }

        this->radio_logs.log_out(inner_id, SensorInit);

        this->radio.stopListening();
        for (int i = 0; i < 50; i++){
            if (this->radio.write(&master_id, sizeof(master_id));){
                break;
            }
        }

        this->radio.startListening();
        this->radio.openReadPipe(INIT_PIPE, (uint8_t*)(INIT_ADDRESS));
        this->radio.openReadPipe(DATA_PIPE, (uint64_t)(MASTER_ID));

        sens_frame.sensor_id = DEFAULT_ID;
        return sens_frame;

    }else if (curr_pipe == DATA_PIPE){
        uint32_t sensor_data[5];

        this->radio.read(&sensor_data, sizeof(sensor_data));
        sens_frame.sensor_id = DEFAULT_ID;

        for (const auto [key, id] : this->reg_sensors){
            if (key == sensor_data[0]){
                sens_frame.sensor_id = id;
                break;
            }
        }

        this->radio_logs.log_out(sens_frame.sensor_id, SensorRead);

        sens_frame.humidity = sensor_data[1];
        sens_frame.temperature = sensor_data[2];
        sens_frame.co2 = 0;
        sens_frame.soil_moisture = 0;

        return sens_frame;
    }

}
