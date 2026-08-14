#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>
#include <stdint.h>

#define PIN_HIGH 1
#define PIN_LOW 0

#define PIN_PORT_B 0
#define PIN_PORT_C 1
#define PIN_PORT_D 2

#define PIN_OUT 0
#define PIN_IN 1

#define EXT_LOW 0x0U
#define EXT_TOGGLE 0x1U
#define EXT_FALLING 0x2U
#define EXT_RISING 0x3U

struct gpio_pin {
  uint8_t pin;
  uint8_t port;
};

uint8_t pin_init(struct gpio_pin *pin, uint8_t dir);
void pin_set_interrupt(struct gpio_pin *pin, uint8_t external);
void pin_set(struct gpio_pin *pin, uint8_t state);
void pin_toggle(struct gpio_pin *pin);
uint8_t pin_read(struct gpio_pin *pin);

#endif