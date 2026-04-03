#include "adc.h"

static uint8_t line_availability = 0;
static uint8_t adc_initialized = 0;

static void adc_init(void);

int8_t init_adc_line(uint8_t line, uint8_t continuous){
  if (line_availability & (1U << line) || line > 7){
    return -1;
  }

  line_availability |= (1U << line);

  if (!adc_initialized){
    adc_init();
  }

  return line;
}

int8_t free_line(uint8_t line){
  if (line > 7){
    return -1;
  }

  line_availability &= ~(1U << line);

  return 0;
}

int8_t read_line(uint8_t line, double* value){
  if (line > 7){
    return -1;
  }

  ADMUX &= ~(0xFU);
  ADMUX |= (line & 0xFU);

  ADCSRA |= (1U << 6);

  while (!(ADCSRA & (1U << 4)));

  uint16_t raw_temp = ADCL;
  raw_temp |= ((ADCH << 8) & 0x300U);

  (*value) = (raw_temp * (5000 / 1024)) / 10;

  return 0;
}

static void adc_init(void){
  adc_initialized = 1;

  ADMUX |= (1U << 6);
  ADCSRA |= (1U << 7);
  ADCSRA |= (0x7U << 0);
}