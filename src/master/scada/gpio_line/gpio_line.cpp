#include "gpio_line.hpp"

GPIOLine::GPIOLINE(uint8_t pin){
    this->pin = pin;

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

    gpiod_request_config_set_consumer(request_conf, (std::string("gpio_pin_") + std::to_string(this->pin)).c_str());

	this->request = gpiod_chip_request_lines(this->chip, request_conf, config);
	gpiod_request_config_free(request_conf);
	gpiod_line_config_free(config);
	gpiod_line_settings_free(settings);
	if (!this->request){
		gpiod_chip_close(this->chip);
		throw std::runtime_error("Failed to request the lines");
	}
}

GPIOLine::~GPIOLine(){
    gpiod_line_request_release(this->request);
	gpiod_chip_close(this->chip);
}

void GPIOLine::write(bool level){
    if (level){
        if (gpiod_line_request_set_value(this->request, this->pin, GPIOD_LINE_VALUE_ACTIVE) < 0){
            throw std::runtime_error("Failed to put the line HIGH");
        }
    }else{
        if (gpiod_line_request_set_value(this->request, this->pin, GPIOD_LINE_VALUE_INACTIVE) < 0){
            throw std::runtime_error("Failed to put the line LOW");
        }
    }
}

bool GPIOLine::read(){
    int line_value = gpiod_line_request_get_value(this->request, this->pin);

    if (line_value < 0){
        throw std::runtime_error("Failed to read line value");
    }

    return line_value == 1;
}