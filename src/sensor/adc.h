#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>

int8_t init_adc_line(uint8_t line, uint8_t continuous);
int8_t free_line(uint8_t line);
int8_t read_line(uint8_t line, double* value);

#endif