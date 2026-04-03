#include "timer.hpp"

Timer::Timer(TIMER_UNITS timer_units){
  switch (timer_units){
    case TIMER_S:
      TCCR1B |= (0x4U << 0);

      OCR1AH = (((62500 - 1) >> 8) & 0xFF);
      OCR1AL = ((62500 - 1) & 0xFF);
      break;

    case TIMER_MS:
      TCCR1B |= (0x2U << 0);

      OCR1AH = (((2000 - 1) >> 8) & 0xFF);
      OCR1AL = ((2000 - 1) & 0xFF);
      break;

    case TIMER_US:
      TCCR1B |= (0x2U << 0);

      OCR1AH = (((2 - 1) >> 8) & 0xFF);
      OCR1AL = ((2 - 1) & 0xFF);
      break;
  }
}

void Timer::start(uint16_t delay){
  this->delay = delay;
  this->passed = 0;

  TCNT1H = 0;
  TCNT1L = 0;

  TCCR1A |= (0x3U << 6);
  TCCR1B |= (1U << 3);

  TIMSK1 |= (1U << 1);
}

void Timer::stop(){
  TCCR1A = 0;
  TCCR1B = 0;

  TIMSK1 = 0;
}

void Timer::inc_passed(){
  this->passed++;
}

uint8_t Timer::comp_passed_delay(){
  if (this->passed >= this->delay){
    this->passed = 0;
    return 1;
  }

  return 0;
}
