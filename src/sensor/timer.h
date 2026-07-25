#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <stdint.h>

#define TIM_OVF 1
#define TIM_PRE_OVF 2

void timer2_init(uint32_t delay_m);
void timer2_start(void);

void timer2_tcnt2_reset(void);
void timer2_delay_counter_reset(void);

uint8_t timer2_inc(void);

void timer1_init(uint8_t delay_s);
void timer1_start(void);
void timer1_stop(void);

uint8_t timer1_inc(void);

#endif