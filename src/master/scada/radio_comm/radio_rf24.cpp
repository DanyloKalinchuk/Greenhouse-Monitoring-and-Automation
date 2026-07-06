#include "radio_rf24.hpp"

void Radio_RF24::sensor_handle_data(uint32_t sensor_data[SENSOR_DATA_SIZE], SENS_FRAME* sens_frame){
    sens_frame->sensor_id = sensor_data[0];
    sens_frame->humidity = sensor_data[1];
    sens_frame->temperature = sensor_data[2];
    sens_frame->co2 = sensor_data[3];
    sens_frame->soil_moisture = sensor_data[4];

    std::string log_message = "Read data from sensor with ID: " + std::to_string(sens_frame->sensor_id);
    Logger::get_instance()->log_out(RADIO_LOGS_PATH, log_message, MsgType::INFO);
}

Radio_RF24::Radio_RF24() : Radio(){
    Logger::get_instance()->log_out("Starting Radio...", MsgType::INFO);
    Logger::get_instance()->log_out(RADIO_LOGS_PATH, "Master started", MsgType::INFO);

    this->radio = RF24(CE, CS);
    this->irq_line = std::make_unique<GPIOLine>(IRQ, true);

    if (!this->radio.begin()){
        Logger::get_instance()->log_out(RADIO_LOGS_PATH, "Master failed to start", MsgType::ERROR);
        throw std::runtime_error("Failed to initialize the radio module");
    }

    this->radio.setPALevel(RF24_PA_LOW);
    this->radio.setDataRate(RF24_250KBPS);
    this->radio.setRetries(10, 15);
    this->radio.maskIRQ(true, true, false);

    this->radio.startListening();
    this->radio.openReadingPipe(INIT_PIPE, (uint8_t*)(INIT_ADDRESS));
    this->radio.openReadingPipe(DATA_PIPE, (uint64_t)(MASTER_ID));

    Logger::get_instance()->log_out("Radio started", MsgType::INFO);
}

SENS_FRAME Radio_RF24::handle_communications(){
    uint8_t curr_pipe;
    SENS_FRAME sens_frame;
    sens_frame.sensor_id = DEFAULT_ID;

    if (!this->irq_line->wait_for_edge_event()){
        return sens_frame;
    }

    bool tx_ok, tx_fail, rx_ready;
    this->radio.whatHappened(tx_ok, tx_fail, rx_ready);

    this->radio.available(&curr_pipe);
    if (curr_pipe == INIT_PIPE){
        const uint8_t master_id = MASTER_ID;
        uint8_t sensor_id;
        this->radio.read(&sensor_id, sizeof(sensor_id));

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

    }else if (curr_pipe == DATA_PIPE){
        uint32_t sensor_data[SENSOR_DATA_SIZE];

        this->radio.read(&sensor_data, sizeof(sensor_data));
        this->sensor_handle_data(sensor_data, &sens_frame);
    }

    return sens_frame;
}