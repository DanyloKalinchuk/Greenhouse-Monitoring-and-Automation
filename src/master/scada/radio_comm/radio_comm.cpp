#include "radio_comm.hpp"

void Radio::read_data_on_disk(){
    std::fstream saved_sensors(SAVE_PATH, std::ios::in | std::ios::binary);
    if (!saved_sensors.is_open() || saved_sensors.peek() == EOF){
        this->radio_logs.log_out(MASTER_ID, MasterFileWarning);
        return;
    }

    this->reg_sensors = {};
    saved_sensors.read(reinterpret_cast<char *>(&this->next_sens_id), sizeof(this->next_sens_id));
    if (saved_sensors.fail()){
        this->radio_logs.log_out(MASTER_ID, MasterFail);
        saved_sensors.close();
        throw std::runtime_error("Failed to read next_sens_id value");
    }

    for (int i = 0; i < (this->next_sens_id - 1); i++){
        uint8_t key, id;
        saved_sensors.read(reinterpret_cast<char *>(&key), sizeof(key));
        saved_sensors.read(reinterpret_cast<char *>(&id), sizeof(id));
        
        if (saved_sensors.fail()){
            this->radio_logs.log_out(MASTER_ID, MasterFail);
            saved_sensors.close();
            throw std::runtime_error("Failed to read id map entry");
        }

        this->reg_sensors[key] = id;
    }

    saved_sensors.close();
}

void Radio::update_data_on_disk(){
    std::fstream saved_sensors(SAVE_PATH, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!saved_sensors.is_open()){
        this->radio_logs.log_out(MASTER_ID, MasterFail);
        throw std::runtime_error((std::string)("Failed to open the save file: ") + (std::string)SAVE_PATH);
    }

    saved_sensors.write(reinterpret_cast<char *>(&this->next_sens_id), sizeof(this->next_sens_id));
    if (saved_sensors.fail()){
        this->radio_logs.log_out(MASTER_ID, MasterFail);
        saved_sensors.close();
        throw std::runtime_error("Failed to write next_sens_id value");
    }

    for (const auto [key, id] : this->reg_sensors){
        saved_sensors.write(reinterpret_cast<const char *>(&key), sizeof(key));
        saved_sensors.write(reinterpret_cast<const char *>(&id), sizeof(id));

        if (saved_sensors.fail()){
            this->radio_logs.log_out(MASTER_ID, MasterFail);
            saved_sensors.close();
            throw std::runtime_error("Failed to write id map entry");
        }
    }

    saved_sensors.close();
}

void Radio::sensor_init(uint8_t sensor_id){
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
        this->radio_logs.log_out(inner_id, SensorRegistered);
    }

    this->radio_logs.log_out(inner_id, SensorInit);
}

void Radio::sensor_handle_data(uint32_t sensor_data[SENSOR_DATA_SIZE], SENS_FRAME* sens_frame){
    sens_frame->sensor_id = DEFAULT_ID;

        for (const auto [key, id] : this->reg_sensors){
            if (key == sensor_data[0]){
                sens_frame->sensor_id = id;
                break;
            }
        }

        this->radio_logs.log_out(sens_frame->sensor_id, SensorRead);

        sens_frame->humidity = sensor_data[1];
        sens_frame->temperature = sensor_data[2];
        sens_frame->co2 = sensor_data[3];
        sens_frame->soil_moisture = sensor_data[4];
}

Radio::Radio(){
    this->radio_logs.log_out(MASTER_ID, MasterStart);
    this->radio = RF24(CE, CS);

    if (!this->radio.begin()){
        this->radio_logs.log_out(MASTER_ID, MasterFail);
        throw std::runtime_error("Failed to initialize the radio module");
    }

    this->read_data_on_disk();

    this->radio.setPALevel(RF24_PA_LOW);
    this->radio.setDataRate(RF24_250KBPS);
    this->radio.setRetries(10, 15);

    this->radio.startListening();
    this->radio.openReadingPipe(INIT_PIPE, (uint8_t*)(INIT_ADDRESS));
    this->radio.openReadingPipe(DATA_PIPE, (uint64_t)(MASTER_ID));
}

Radio::Radio(uint8_t dummy){
    this->radio_logs.log_out(MASTER_ID, MasterStart);
    this->read_data_on_disk();
}

Radio::~Radio(){
    update_data_on_disk();
}

SENS_FRAME Radio::handle_communications(){
    uint8_t curr_pipe;
    SENS_FRAME sens_frame;

    while (!this->radio.available(&curr_pipe));

    if (curr_pipe == INIT_PIPE){
        const uint8_t master_id = MASTER_ID;
        uint8_t sensor_id;
        this->radio.read(&sensor_id, sizeof(sensor_id));

        this->sensor_init(sensor_id);

        this->radio.stopListening();
        this->radio.openWritingPipe((uint64_t)(sensor_id));
        for (int i = 0; i < 50; i++){
            if (this->radio.write(&master_id, sizeof(master_id))){
                break;
            }
        }

        this->radio.startListening();
        this->radio.openReadingPipe(INIT_PIPE, (uint8_t*)(INIT_ADDRESS));
        this->radio.openReadingPipe(DATA_PIPE, (uint64_t)(MASTER_ID));

        sens_frame.sensor_id = DEFAULT_ID;

    }else if (curr_pipe == DATA_PIPE){
        uint32_t sensor_data[SENSOR_DATA_SIZE];

        this->radio.read(&sensor_data, sizeof(sensor_data));
        this->sensor_handle_data(sensor_data, &sens_frame);
    }

    return sens_frame;
}
