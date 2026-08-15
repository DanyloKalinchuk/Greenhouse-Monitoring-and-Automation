#include "gpio.h"

uint8_t pin_init(struct gpio_pin *pin, uint8_t dir){
  if (pin->pin > 7 || pin->pin < 0){
    return -1;
  }

  switch (pin->port){
    case PIN_PORT_D:
      if (dir == PIN_OUT){
        DDRD |= (1U << pin->pin);
      }else {
        DDRD &= ~(1U << pin->pin);
      }
      break;

    case PIN_PORT_B:
      if (dir == PIN_OUT){
        DDRB |= (1U << pin->pin);
      }else{
        DDRB &= ~(1U << pin->pin);
      }
      break;

    case PIN_PORT_C:
      if (dir == PIN_OUT){
        DDRC |= (1U << pin->pin);
      }else{
        DDRC &= ~(1U << pin->pin);
      }
      break;

    default:
      break;
  }

  return 0;
}

void pin_set_interrupt(struct gpio_pin *pin, uint8_t external){
  switch (pin->port){
    case PIN_PORT_B:
      PCMSK0 |= (1U << pin->pin);
      break;
    
    case PIN_PORT_C:
      PCMSK1 |= (1U << pin->pin);
      break;

    case PIN_PORT_D:
      if (external && (pin->pin == 2 || pin->pin == 3)){
        EICRA |= (external & 0x3U) << (pin->pin == 2 ? 0 : 2);
        EIMSK |= 1U << (pin->pin - 2);
      }else{
        PCMSK2 |= (1U << pin->pin);
      }
      break;

    default:
      break;
  }


}

void pin_set(struct gpio_pin *pin, uint8_t state){
  switch (pin->port){
    case PIN_PORT_D:
      if (state == PIN_HIGH){
        PORTD |= (1U << pin->pin);
      }else {
        PORTD &= ~(1U << pin->pin);
      }
      break;

    case PIN_PORT_B:
      if (state == PIN_HIGH){
        PORTB |= (1U << pin->pin);
      }else{
        PORTB &= ~(1U << pin->pin);
      }
      break;

    case PIN_PORT_C:
      if (state == PIN_HIGH){
        PORTC |= (1U << pin->pin);
      }else{
        PORTC &= ~(1U << pin->pin);
      }
      break;

    default:
      break;
  }
}

void pin_toggle(struct gpio_pin *pin){
  switch (pin->port){
    case PIN_PORT_D:
      PORTD ^= (1U << pin->pin);
      break;

    case PIN_PORT_B:
      PORTB ^= (1U << pin->pin); 
      break;

    case PIN_PORT_C:
      PORTC ^= (1U << pin->pin);
      break;

    default:
      break;
  }
}

uint8_t pin_read(struct gpio_pin *pin){
  switch (pin->port){
    case PIN_PORT_D:
      if (PIND & (1U << pin->pin)){
        return 1;
      }
      break;
    
    case PIN_PORT_B:
      if (PINB & (1U << pin->pin)){
        return 1;
      }
      break;

    case PIN_PORT_C:
      if (PINC & (1U << pin->pin)){
        return 1;
      }
      break;

    default:
      break;
  }

  return 0;
}