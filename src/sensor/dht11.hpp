#ifndef DHT11_HPP
#define DHT11_HPP

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define READ_REF_SUM_ERROR (uint32_t)(0xAAAAU) ///< Is returned in a DHT11_DATA representaion if the DHT11::read() fails

/**
 * \brief Structure that contains Temperature and Humidity values
 */
struct DHT11_DATA {
    double temperature; ///< Temperature in Celsius
    double humidity; ///< Relative Humidity in percent
};

/**
 * \brief Class for managing DHT11 sensor
 */
class DHT11{
    const uint8_t pin;

    protected:
    void init_read(); ///< Initiates a DHT11 measurement sequence, must be called before read_byte()
    uint8_t read_byte(); ///< Reads 8-bit sequence from DHT11 data line
    DHT11_DATA raw_data_to_struct(uint32_t raw_data); ///< Converts 32-bit value into two 16-bit values as a DHT11_DATA structure

    public:

    /**
     * \brief Construct DHT11 object
     * The constructed object represents a DH11 sensor connected to a provided 'in_pin'
     * \warning 'in_pin' should be located on a port D (from 1 to 7)
     * 
     * \param in_pin The pin to which the sensor is connected
     */
    DHT11(uint8_t in_pin);

    /**
     * \brief Reads DH11 sensor values
     * 
     * \return Temperature and Humidity packed as a DH11_DATA structure instance
     */
    DHT11_DATA read();
};

#endif