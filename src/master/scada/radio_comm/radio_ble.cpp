#include "radio_ble.hpp"

void Radio_BLE::interfaces_added_handler(sdbus::Signal signal){
    sdbus::ObjectPath device_path;
    signal >> device_path;

    std::lock_guard<std::mutex> queue_lock(this->queue_mtx);
    this->queue.push(device_path);
    this->queue_is_ready.store(true);
    this->queue_cv.notify_all();
}

Radio_BLE::Radio_BLE() : Radio(){
    this->conn = sdbus::createSystemBusConnection();

    auto obj_manager_proxy = sdbus::createProxy(*this->conn, Radio_BLE::bluez_service, Radio_BLE::obj_manager_path);
    
    sdbus::SignalName interfaces_added_signal{"InterfacesAdded"};
    obj_manager_proxy->registerSignalHandler(obj_manager_interface, interfaces_added_signal, 
        [this] (sdbus::Signal signal) {this->interfaces_added_handler(signal)});
    
    auto adapter_proxy = sdbus::createProxy(*this->conn, Radio_BLE::bluez_service, Radio_BLE::adapter_path);

    std::map<std::string, sdbus::Variant> filter;
    filter["UUIDs"] = sdbus::Variant{std::vector<std::string>{UUID_FILTER}};

    adapter_proxy->callMethod("SetDiscoveryFilter").onInterface(Radio_BLE::adapter_interface).withArguments(filter);
    adapter_proxy->callMethod("StartDiscovery").onInterface(Radio_BLE::adapter_interface);
}

Radio_BLE::~Radio_BLE(){
    this->queue_is_ready.store(true);
    this->queue_cv.notify_all();

    auto adapter_proxy = sdbus::createProxy(*this->conn, Radio_BLE::bluez_service, Radio_BLE::adapter_path);
    adapter_proxy->callMethod("StopDiscovery").onInterface(Radio_BLE::adapter_interface);
}

SENS_FRAME Radio_BLE::handle_communications(){
    SENS_FRAME frame = {0, 0, 0, 0, 0};

    if (!this->queue_is_ready.load()){
        std::unique_lock<std::mutex> cv_lock(this->queue_cv_mtx);
        this->queue_cv.wait(cv_lock, [this] {return this->queue_is_ready.load();});
    }

    sdbus::ObjectPath device_path;
    {
        std::lock_guard<std::mutex> queue_lock(this->queue_mtx);

        if (this->device_queue.empty()){
            return frame;
        }

        device_path = this->device_queue.front();
        this->device_queue.pop();

        if (this->device_queue.empty()){
            this->queue_is_ready.store(false);
        }
    }

    auto device_proxy = sdbus::createProxy(*this->conn, Radio_BLE::bluez_service, device_path);
    sdbus::Variant property = device_proxy->getProperty("ServiceData").onInterface(Radio_BLE::device_interface);
    std::map<std::string, sdbus::Variant> service_data = property.get<std::map<std::string, std::Variant>>();
    std::vector<uint8_t> data = service_data.at(UUID_FILTER).get<std::vector<uint8_t>>();

    frame.sensor_id = data[0];
    frame.temperature = data[1];
    frame.humidity = data[2];
    frame.co2 = data[3];
    frame.soil_moisture = data[4];

    return frame;
}
