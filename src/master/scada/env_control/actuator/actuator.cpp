#include "actuator.hpp"

void Actuator::handle_changes(int16_t param_value){
    
}

Actuator::Actuator(uint8_t pin, int16_t init_perf, uint8_t init_error) : line_initialized(true){
    this->pin = pin;
    this->perf = init_perf;
    this->error = init_error;

    this->chip = gpiod_chip_open("/dev/gpiochip0");
	if (!this->chip){
		throw std::runtime_error("Failed to open the chip");
	}

    struct gpiod_line_settings *settings = gpiod_line_settings_new();
	if (!settings){
		gpiod_chip_close(this->chip);
		throw std::runtime_error("Failed to creat line_settings");
	}

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
	gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);

    struct gpiod_line_config *config = gpiod_line_config_new();
	if (!config){
		gpiod_line_settings_free(settings);
		gpiod_chip_close(this->chip);
		throw std::runtime_error("Failed to create line_config");
	}

    unsigned int offsets[] = {this->pin};
	if (gpiod_line_config_add_line_settings(config, offsets, 1, settings) < 0){
		gpiod_line_config_free(config);
		gpiod_line_settings_free(settings);
		gpiod_chip_close(this->chip);
		throw std::runtime_error("Failed to add line settings");
	}

    struct gpiod_request_config *request_conf = gpiod_request_config_new();
	if (!request_conf){
		gpiod_line_config_free(config);
		gpiod_line_settings_free(settings);
		gpiod_chip_close(this->chip);
		throw std::runtime_error("Failed to create request_config");
	}

    gpiod_request_config_set_consumer(request_conf, (std::string("actuator_pin_") + std::to_string(this->pin)).c_str());

	this->request = gpiod_chip_request_lines(this->chip, request_conf, config);
	gpiod_request_config_free(request_conf);
	gpiod_line_config_free(config);
	gpiod_line_settings_free(settings);
	if (!this->request){
		gpiod_chip_close(this->chip);
		throw std::runtime_error("Failed to request the lines");
	}
}

Actuator::Actuator(int16_t init_perf, uint8_t init_error) : line_initialized(false){
	this->perf = init_perf;
	this->error = init_error;
}

Actuator::~Actuator(){
    if (this->line_initialized){
		gpiod_line_request_release(this->request);
		gpiod_chip_close(this->chip);
	}
}

void Actuator::set_target(int16_t perf, uint8_t error){
    this->perf = perf;
    this->error = error;
}