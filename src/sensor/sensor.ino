/**
 * \file
 * \brief Main file for the Sensor device
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <SPI.h>
#include <RF24.h>
#include <stdint.h>

#include "dht11.hpp"
#include "eeprom_ids.hpp"

extern "C"{
  #include "adc.h"
  #include "power_mode.h"
  #include "timer.h"
  #include "gpio.h"
}

#define INIT_PIPE 0 ///< Pipe number for the Sensor initialization

#define PERIPH_PWR_PIN 0 ///< Pin that switches peripheral on/off
#define RESET_BTN_PIN 2 ///< Pin Reset button is connected to
#define LED_ACTIVE_PIN 5 ///< Pin connected to led that signals activity (waked up state or before waking up with higher current consumption)
#define DHT11_PIN 4 ///< Pin to which the DHT11 sensor is connected
#define MOIST_PIN 2 ///< Soil Moisture sensor pin
#define CO2_PIN 3 ///< CO2 sensor pin
#define CE 7 ///< CE line of the nRF24L01 module
#define CS 10 ///< CS line of the nRF24L01 module

#define ADC_MAX_VAL 1023.0

#define SENSOR_DELAY 5 ///< Delay in minutes for the sensor data sending loop
#define PERIPH_PREP_PERIOD 40 ///< Time in seconds for external peripheral to be turned on before waking up
#define RESET_BTN_DELAY 10 ///< Time in seconds the reset button should be hold to reset sensor with setting master id to MASTER_DEFAULT

const uint8_t init_address[] = "init_address"; ///< Address for Sensor initialization. Must be identical to the INIT_ADDRESS in \ref radio_comm.hpp
mstr_sens_ids::M_S_IDS ms_ids = mstr_sens_ids::read();
RF24 radio = RF24(CE, CS);

DHT11 dht = DHT11(DHT11_PIN);
DHT11_DATA dht_data;

uint8_t moist_line = init_adc_line(MOIST_PIN);
uint8_t co2_line = init_adc_line(CO2_PIN);
uint16_t adc_raw;

volatile uint8_t timer2_flag = 1; ///< Flag set by the Timer2 ISR
volatile uint8_t timer1_flag = 0; ///< Flag set if Timer 1 is active
volatile uint8_t reset_flag = 0; ///< Resets sensor if set

const struct gpio_pin periph_pin = {
  .pin = PERIPH_PWR_PIN,
  .port = PIN_PORT_B
};

const struct gpio_pin reset_pin = {
  .pin = RESET_BTN_PIN,
  .port = PIN_PORT_D
};

const struct gpio_pin led_active = {
  .pin = LED_ACTIVE_PIN,
  .port = PIN_PORT_D
};

/**
 * \brief Initializes Sensor
 * Starts Sensor initialization sequence if Master ID is set to default. Starts Timer and WDT.
 */
void setup() {
  if (!radio.begin()){
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(10, 15);

  if (ms_ids.master == MASTER_DEFAULT){
    radio.stopListening();
    radio.openWritingPipe(init_address);
    while (!radio.write(&ms_ids.sensor, sizeof(ms_ids.sensor)));
    radio.startListening();

    radio.openReadingPipe(INIT_PIPE, (uint64_t)ms_ids.sensor);
    while(!radio.available(INIT_PIPE));
    radio.read(&ms_ids.master, sizeof(ms_ids.master));
    
    mstr_sens_ids::update_master(ms_ids.master);
  }

  radio.stopListening();
  radio.openWritingPipe((uint64_t)ms_ids.master);

  pin_init(&periph_pin, PIN_OUT);
  pin_set(&periph_pin, PIN_HIGH);

  pin_init(&led_active, PIN_OUT);
  pin_set(&led_active, PIN_HIGH);

  pin_init(&reset_pin, PIN_IN);
  pin_set_interrupt(&reset_pin, EXT_TOGGLE);

  timer1_init(RESET_BTN_DELAY);

  timer2_init(SENSOR_DELAY, PERIPH_PREP_PERIOD);
  timer2_start();
  sei();

  _delay_ms(PERIPH_PREP_PERIOD * 1000);
}

/**
 * \brief Sensor main loop
 * Resets WDT. If timer_flag is set, reads and sends sensor data to Master
 */
void loop() {
  wdt_enable(WDTO_2S);

  if (timer2_flag){
    dht_data = dht.read();
    uint32_t data_to_send[5];

    read_line(moist_line, &adc_raw);
    uint32_t soil_moisture = (adc_raw * 100) / ADC_MAX_VAL;

    read_line(co2_line, &adc_raw);
    uint32_t co2 = (adc_raw * 100) / ADC_MAX_VAL;

    data_to_send[0] = (uint32_t)(ms_ids.sensor);
    data_to_send[1] = (uint32_t)(dht_data.humidity);
    data_to_send[2] = (uint32_t)(dht_data.temperature);
    data_to_send[3] = co2;
    data_to_send[4] = soil_moisture;

    radio.write(data_to_send, sizeof(data_to_send));

    radio.powerDown();
    pin_set(&periph_pin, PIN_LOW);
    timer2_flag = 0;
    timer2_delay_counter_reset();

    pin_set(&led_active, PIN_LOW);
  }

  while (timer1_flag){
    pin_toggle(&led_active);
    wdt_reset();

    if (reset_flag){
      pin_set(&led_active, PIN_LOW);
      mstr_sens_ids::update_master(MASTER_DEFAULT);
      while (1){}
    }

    _delay_ms(500);
  }
  pin_set(&led_active, PIN_LOW);

  wdt_disable();
  timer2_tcnt2_reset();
  pm_sleep(POWER_SAVE_MODE);
}

ISR(TIMER2_OVF_vect){
  switch (timer2_inc()){
    case TIM_OVF:
      timer2_flag = 1;
      break;
    case TIM_PRE_OVF:
      radio.powerUp();

      pin_set(&periph_pin, PIN_HIGH);
      pin_set(&led_active, PIN_HIGH);
      break;
    default:
      break;
  }
}

ISR(TIMER1_OVF_vect){
  if (timer1_inc()){
    reset_flag = 1;
  }
}

ISR(INT0_vect){
  if (pin_read(&reset_pin)){
    timer1_flag = 1;
    timer1_start();
  }else{
    timer1_stop();
    timer1_flag = 0;
  }
}
