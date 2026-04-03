#include "dht11.hpp"

void DHT11::init_read(){
  DDRD |= (1U << this->pin);
  PORTD &= ~(1U << this->pin);
  _delay_ms(18);
  PORTD |= (1U << this->pin);
  DDRD &= ~(1U << this->pin);

  while(PIND & (1 << this->pin));
  _delay_us(160);
}

uint8_t DHT11::read_byte(){
  uint8_t byte = 0;
  for (int i = 0; i <8; i++){
    while(PIND & (1U << this->pin));
    _delay_us(10);
    while(!(PIND & (1U << this->pin)));
    _delay_us(28);

    byte <<= 1;
    if (PIND & (1U << this->pin)){
      byte |= 1;
    }
  }

  return byte;
}

DHT11_DATA DHT11::raw_data_to_struct(uint32_t raw_data){
  DHT11_DATA dht11_data;

  dht11_data.humidity = (raw_data & 0xFF) + (((raw_data >> 8) & 0xFF) / 100.0);
  dht11_data.temperature = ((raw_data >> 16) & 0xFF) + (((raw_data >> 24) & 0xFF) / 100.0);

  return dht11_data;
}

DHT11::DHT11(uint8_t in_pin) : 
  pin(in_pin) {
    DDRD &= ~(1U << this->pin);
    PORTD |= (1U << this->pin);
  }

DHT11_DATA DHT11::read(){
  uint32_t raw_data = 0;
  uint8_t checksum = 0;
  this->init_read();

  for (int i = 0; i < 4; i++){
    uint8_t data_byte = this->read_byte();
    checksum += data_byte;
    raw_data |= ((uint32_t)data_byte << (i * 8));
  }

  uint8_t ref_sum = this->read_byte();

  if (checksum == ref_sum){
    return this->raw_data_to_struct(raw_data);
  }

  return this->raw_data_to_struct(READ_REF_SUM_ERROR);
}