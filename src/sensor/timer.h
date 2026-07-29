#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <stdint.h>

#define TIM_OVF 1
#define TIM_PRE_OVF 2

/**
 * \brief Initializes Timer2
 * \note To get the timer's state, timer2_inc() should be called inside TIMER2_OVF_vect body
 * 
 * \param delay_m Delay before TIM_OVF is returned by timer2_inc()
 * \param pre_ovf_delay_s Interval before TIM_OVF the TIM_PRE_OVF is returned by timer2_inc()
 */
void timer2_init(uint32_t delay_m, uint32_t pre_ovf_delay_s);

/// \brief Starts Timer2 
void timer2_start(void);

/// \brief Sets TCNT2 to zero 
void timer2_tcnt2_reset(void);

/// \brief Sets timer2_delay_counter to zero
void timer2_delay_counter_reset(void);

/**
 * \brief Increments timer2_delay_counter value
 * 
 * \return
 *      TIM_OVF if timer2_delay_counter >= timer2_delay_ovf
 *      else TIM_PRE_OVF if timer2_delay_counter >= timer2_delay_pre_ovf
 *      otherwise returns 0
 */
uint8_t timer2_inc(void);

/**
 * \brief Initiaizes Timer1
 * \note To get the timer's state, timer1_inc() should be called inside TIMER1_OVF_vect body
 * 
 * \param delay_s Delay before TIM_OVF is returned by timer2_inc()
 */
void timer1_init(uint32_t delay_s);

/// \brief Starts Timer1
void timer1_start(void);

/// \brief Stops Timer1
void timer1_stop(void);

/**
 * \brief Increments timer1_delay_counter value
 * 
 * \return
 *      TIM_OVF if timer1_delay_counter >= timer1_delay_ovf
 *      otherwise returns 0
 */
uint8_t timer1_inc(void);

#endif