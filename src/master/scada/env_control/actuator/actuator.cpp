#include "actuator.hpp"

void Actuator::handle_changes(int16_t param_value) {}

Actuator::Actuator(std::unique_ptr<Line> line, int16_t init_perf, uint8_t init_error) : line_initialized(true){
    this->line = std::move(line);
	this->perf = init_perf;
	this->error = init_error;
}

Actuator::Actuator(int16_t init_perf, uint8_t init_error) : line_initialized(false){
	this->perf = init_perf;
	this->error = init_error;
}

void Actuator::set_target(int16_t perf, uint8_t error){
    this->perf = perf;
    this->error = error;
}