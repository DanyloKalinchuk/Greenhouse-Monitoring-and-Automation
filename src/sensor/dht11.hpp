#ifndef DHT11_HPP
#define DHT11_HPP

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define READ_REF_SUM_ERROR (uint32_t)(0xAAAAU)

struct DHT11_DATA {
    double temperature;
    double humidity;
};

class DHT11{
    const uint8_t pin;

    protected:
    void init_read();
    uint8_t read_byte();
    DHT11_DATA raw_data_to_struct(uint32_t raw_data);

    public:
    DHT11(uint8_t in_pin); //Pins of port D from 1 to 7

    DHT11_DATA read();
};

#endif