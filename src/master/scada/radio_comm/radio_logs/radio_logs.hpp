#ifndef RADIO_LOGS
#define RADIO_LOGS

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdint>

/// \brief Types of logging messages
enum MsgType{
    MasterStart, ///< Logged on Master device start
    MasterFail, ///< Logged on logic fail on the Master side
    MasterFileWarning, ///< Logged if the saved_sensors file is empty
    SensorRegistered, ///< Logged on Sensor registration
    SensorInit, ///< Logged on Sensor initialization
    SensorRead ///< Logged on Sensor data received
};

/// \brief Class for logging data operations
class RadioLogs{
    std::fstream log_file;
    
    public:

    /**
     * \brief Constructs RadioLogs object
     * 
     * \param path Defines output path
     */
    RadioLogs(std::string path);
    ~RadioLogs();

    /**
     * \brief Logs out a message
     * 
     * \param initiator_id ID of a device that the message is refered to
     * \param msg_type Type of the message
     */
    void log_out(uint8_t initiator_id, MsgType msg_type);
};

#endif