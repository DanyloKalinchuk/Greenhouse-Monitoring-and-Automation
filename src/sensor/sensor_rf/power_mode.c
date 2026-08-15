#include "power_mode.h"

#define SE (1U << 0)

#define PRR_TWI (1U << 7)
#define PRR_TIM0 (1U << 5)
#define PRR_TIM1 (1U << 3)
#define PRR_USART0 (1U << 1)
#define PRR_VALUE PRR_TWI | PRR_TIM0 | PRR_USART0

void pm_sleep(uint8_t power_mode){
  SMCR |= power_mode;
  PRR = PRR_VALUE;

  SMCR |= SE;

  asm volatile("sleep");

  SMCR &= ~(SE);
}
