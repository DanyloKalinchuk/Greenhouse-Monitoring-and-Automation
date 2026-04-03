#ifndef TIMER_HPP
#define TIMER_HPP

#include <avr/io.h>
#include <avr/interrupt.h>

enum TIMER_UNITS{
  TIMER_S,
  TIMER_MS,
  TIMER_US
};

class Timer{
  uint16_t delay;
  uint16_t passed;

  public:
  Timer(TIMER_UNITS timer_units);

  void start(uint16_t delay);
  void stop();

  void inc_passed();
  uint8_t comp_passed_delay();
};

#endif