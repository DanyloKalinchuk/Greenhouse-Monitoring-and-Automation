#ifndef TIMER_HPP
#define TIMER_HPP

#include <avr/io.h>
#include <avr/interrupt.h>

/// \brief Values for setting timer units
enum TIMER_UNITS{
  TIMER_S, ///< sets timer units to seconds
  TIMER_MS, ///< sets timer units to milliseconds
  TIMER_US ///< sets timer units to microseconds
};

/**
 * \brief Class for timer management
 * 
 * \warning Is to use with ISR(TIMER1_COMPA_vect) only
 */
class Timer{
  uint16_t delay; ///< Delay meassured by Timer
  uint16_t passed; ///< Time passed from the last reset

  public:
  /**
   * \brief Constructs Timer object
   * 
   * \param timer_units Units used by Timer object (defined in TIMER_UNITS)
   */
  Timer(TIMER_UNITS timer_units);

  /**
   * \brief Starts Timer
   * 
   * \param delay Sets the delay that should be meassured
   */
  void start(uint16_t delay);

  /// \brief Stops Timer 
  void stop();

  /**
   * \brief Increments the passed variable
   * 
   * \warning Must be called from ISR(TIMER1_COMPA_vect) only
   */
  void inc_passed();

  /**
   * \brief Checks if the delay has passed
   * 
   * \return '1' if True, '0' if False. Should be used as a boolean
   */
  uint8_t comp_passed_delay();
};

#endif