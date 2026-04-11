#ifndef RADIO_LOGS
#define RADIO_LOGS

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdint>


enum MsgType{
    MasterStart,
    MasterFail,
    SensorRegistered,
    SensorInit,
    SensorRead
};


class RadioLogs{
    std::fstream log_file;
    
    public:
    RadioLogs(std::string path);
    ~RadioLogs();

    void log_out(uint8_t initiator_id, MsgType msg_type);
};

#endif