#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <cstdint>

#define DEFAULT_OUTPUT_PATH "logs/logs.log"

/// \brief Types of logging messages
enum MsgType {
    INFO,
    WARNING,
    ERROR
};

/// \brief Contains path to an output file and text for a log message
struct LogQObj{
    std::string path; ///< Path to the output file
    std::string message; ///< Log message
};

class Logger{
    static Logger* instance_;

    std::queue<LogQObj> log_queue; ///< Queueu of messages to log out
    std::condition_variable queue_cv; ///< Conditional variable for log_queue handling
    std::mutex queue_mtx; ///< Prevents concurrent access to log_queue from multiple threads
    std::atomic<bool> queue_is_ready; ///< Prevents spurious awakenings

    std::thread log_thread; ///< Logger routine thread
    std::atomic<bool> log_on; ///< Keeps log_thread alive
    std::mutex log_mtx; ///< Used for the queue_cv wait

    std::string default_ouput_path; ///< Path to the default output file
    std::map<std::string, std::fstream> output_files; ///< Ouput files indexed by their paths
    std::mutex output_files_mtx; ///< Prevents concurrent access to output_files from multiple threads

    /**
     * \brief Opens a new output file
     * 
     * \param path Path to the output file
     */
    void open_output(std::string path);

    /**
     * \brief Constructs log message
     * 
     * \param message Log message text
     * \param msg_type Log message type
     * 
     * \return Log message as a std::string
     */
    std::string construct_message(std::string message, MsgType msg_type);

    /**
     * \brief Writes log message to the corresponding output file
     * 
     * \param queue_object LogQObj that contains path to an output file and text for a log message
     */
    void write_message(struct LogQObj queue_object);

    protected:
    /// \brief Logger routine loop
    void queue_handling();

    /**
     * \brief Constructs Logger object
     * 
     * \param path Path to the default output file
     */
    Logger(std::string path);
    ~Logger();

    public:
    static Logger* get_instance();

    /**
     * \brief Logs out a message into the default output file
     * 
     * \param message Content of the message
     * \param msg_type Message type
     */
    void log_out(std::string message, MsgType msg_type);

    /**
     * \brief Logs out a message into the specified output file
     * 
     * \param path Path to the output file
     * \param message Content of the message
     * \param msg_type Message type
     */
    void log_out(std::string path, std::string message, MsgType msg_type);
};

#endif