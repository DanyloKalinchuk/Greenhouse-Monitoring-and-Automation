#include "timer.h"

#define TCCR2B_CS (0x07U)
#define TCCR0B_CS (0x05U)
#define TIMSK2_TOIE2 (1U << 0)
#define TIMSK0_TOIE0 (1U << 0)

#define OVFS_PER_SEC (F_CPU / 1024 / 256)
#define OVFS_PER_MIN OVFS_PER_SEC * 60
#define T1_OVFS_PER_SEC (F_CPU / 1024)

static uint32_t timer2_delay_ovf;
static uint32_t timer2_delay_pre_ovf;
static uint32_t timer2_delay_counter = 0;

static uint16_t timer1_delay_ovf;
static uint16_t timer1_delay_counter;

void timer2_init(uint32_t delay_m){
  timer2_delay_ovf = delay_m * OVFS_PER_MIN;
  timer2_delay_pre_ovf = (delay_m - 1) * OVFS_PER_MIN;

  TCCR2A = 0x00;
  TCCR2B = TCCR2B_CS;
}

void timer2_start(void){
  TCNT2 = 0;

  TIMSK2 |= TIMSK2_TOIE2;
}

void timer2_tcnt2_reset(void){
  TCNT2 = 0;
}

void timer2_delay_counter_reset(void){
  timer2_delay_counter = 0;
}

uint8_t timer2_inc(void){
  timer2_delay_counter++;

  if (timer2_delay_counter >= timer2_delay_ovf){
    timer2_delay_counter = 0;
    return TIM_OVF;
  }else if (timer2_delay_counter >= timer2_delay_pre_ovf){
    return TIM_PRE_OVF;
  }

  return 0;
}

void timer1_init(uint8_t delay_s){
  timer1_delay_ovf = ((uint32_t)delay_s * T1_OVFS_PER_SEC) / 65536;

  TCCR1A = 0x00;
  TCCR1B = TCCR0B_CS;
}

void timer1_start(void){
  TCNT1 = 0;
  timer1_delay_counter = 0;

  TIMSK1 |= TIMSK0_TOIE0;
}

void timer1_stop(void){
  TIMSK1 &= ~(TIMSK0_TOIE0);
}

uint8_t timer1_inc(void){
  timer1_delay_counter++;

  if (timer1_delay_counter >= timer1_delay_ovf){
    return TIM_OVF;
  }

  return 0;
}