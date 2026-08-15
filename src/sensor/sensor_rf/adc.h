/** 
 * \file 
 * \brief Header that contains function for managing ADC pins operations
 */

#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>

/**
 * \brief Checks if the provided ADC line exists, its availability and initializes it
 *
 * \param line The line that should be aquired
 * \return On success returns the initialized line. On failure return -1
 */
int8_t init_adc_line(uint8_t line);

/**
 * \brief Make ADC line available to be aqcuired by init_adc_line()
 * 
 * \param line The line that should be freed
 * \return '0' on success (or if the line was free), '-1' if a line doesn't exist
 */
int8_t free_line(uint8_t line);

/**
 * \brief Reads the raw ADC value on the given line
 *
 * \param[in] line The line that should be read
 * \param[out] value Pointer to a variable where ADC value should be stored
 * \return '0' on success. '-1' if a line doesn't exist
 */
int8_t read_line(uint8_t line, uint16_t* value);

#endif