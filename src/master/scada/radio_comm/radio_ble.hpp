#ifndef RADIO_BLE_HPP
#define RADIO_BLE_HPP

#include "radio_comm.hpp"
#include <sdbus-c++/sdbus-c++.h>
#include <cstdint>
#include <queue>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#define UUID_FILTER "0000181a-0000-1000-0000-00005f9b34fb"

class Radio_BLE : public Radio{
    std::queue<sdbus::ObjectPath> device_queue;
    std::mutex queue_mtx;
    std::atomic<bool> queue_is_ready;
    std::condition_variable queue_cv;
    std::mutex queue_cv_mtx;

    std::unique_ptr<sdbus::IConnection> conn;

    void interfaces_added_handler(sdbus::Signal signal);

    public:
    Radio_BLE();
    ~Radio_BLE();

    SENS_FRAME handle_communications() override;
};

#endif