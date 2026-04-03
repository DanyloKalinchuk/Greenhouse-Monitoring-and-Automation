#ifndef RADIO_LOGS
#define RADIO_LOGS

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>


enum MsgType{
    MasterStart,
    MasterFail,
    SensorInit,
    SensorRead
};


class RadioLogs{
    std::fstream log_file;
    time_t timestamp time(NULL);
    struct tm datetime = *localtime(&timestamp);
    
    public:
    RadioLogs(std::string path);
    ~RadioLogs();

    void log_out(uint8_t initiator_id, MsgType msg_type);
};

#endif