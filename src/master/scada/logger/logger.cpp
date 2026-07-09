#include "logger.hpp"

Logger* Logger::instance_ = nullptr;

Logger* Logger::get_instance(){
    if (instance_ != nullptr){
        return instance_;
    }
    instance_ = new Logger(DEFAULT_OUTPUT_PATH);

    return instance_;
}

void Logger::open_output(std::string path){
    if (this->output_files.find(path) != this->output_files.end()){
        this->log_out(path, "The output file was attempted to be opened again", MsgType::WARNING);
        return;
    }

    this->output_files[path] = std::fstream((LOGS_BASE_DIR + path), std::ios::out | std::ios::app);

    if (!this->output_files[path].is_open()){
        throw std::runtime_error("Failed to open a logging file; Path: " + path);
    }
}

std::string Logger::construct_message(std::string message, MsgType msg_type){
    time_t timestamp = std::time(NULL);
    struct tm datetime = *localtime(&timestamp);
    std::string log_msg = asctime(&datetime);
    log_msg.pop_back();

    log_msg += " | ";
    switch (msg_type){
        case MsgType::ERROR:
            log_msg += "ERROR | ";
            break;
        case MsgType::WARNING:
            log_msg += "WARNING | ";
            break;
        default:
            log_msg += "INFO | ";
            break;
    }

    log_msg += message + " |\n";

    return log_msg;
}

void Logger::write_message(struct LogQObj queue_object){
    std::string message = this->construct_message(queue_object.message, queue_object.msg_type);

    std::lock_guard<std::mutex> output_lock(this->output_files_mtx);

    this->output_files[queue_object.path] << queue_object.message;
    this->output_files[queue_object.path].flush();
}

void Logger::queue_handling(){
    while (this->log_on.load()){
        std::unique_lock<std::mutex> log_lock(this->log_mtx);
        this->queue_cv.wait(log_lock, [this] {return this->queue_is_ready.load();});

        while (!this->log_queue.empty()){
            struct LogQObj queue_object;
            {
                std::lock_guard<std::mutex> queue_lock(this->queue_mtx);
                queue_object  = this->log_queue.front();
                this->log_queue.pop();
            }

            this->write_message(queue_object);
        }
        this->queue_is_ready.store(false);
    }
}

Logger::Logger(std::string path){
    this->default_ouput_path = path;
    this->open_output(path);

    this->log_on.store(true);
    this->log_thread = std::thread(&Logger::queue_handling, this);
}

Logger::~Logger(){
    this->log_on.store(false);
    this->queue_is_ready.store(true);
    this->queue_cv.notify_one();

    if (this->log_thread.joinable()){
        this->log_thread.join();
    }

    for (auto& [path, output_file] : this->output_files){
        output_file.close();
    }
}

void Logger::log_out(std::string message, MsgType msg_type){
    struct LogQObj queue_object = {
        .path = DEFAULT_OUTPUT_PATH,
        .message = message,
        .msg_type = msg_type
    };

    std::lock_guard<std::mutex> queue_lock(this->queue_mtx);
    this->log_queue.push(queue_object);
    this->queue_is_ready.store(true);
    this->queue_cv.notify_one();
}

void Logger::log_out(std::string path, std::string message, MsgType msg_type){
    {   
        std::lock_guard<std::mutex> output_lock(this->output_files_mtx);
        if (this->output_files.find(path) == this->output_files.end()){
            this->open_output(path);
        }
    }

    struct LogQObj queue_object = {
        .path = path,
        .message = message,
        .msg_type = msg_type
    };

    std::lock_guard<std::mutex> queue_lock(this->queue_mtx);
    this->log_queue.push(queue_object);
    this->queue_is_ready.store(true);
    this->queue_cv.notify_one();
}