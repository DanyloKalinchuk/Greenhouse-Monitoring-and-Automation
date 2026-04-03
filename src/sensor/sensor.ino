#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <SPI.h>
#include <RF24.h>
#include <stdint.h>

#include "dht11.hpp"
#include "timer.hpp"
#include "eeprom_ids.hpp"

extern "C"{
  #include "adc.h"
}

#define INIT_PIPE 0

#define DHT11_PIN 4
#define CE 7
#define CS 10

void init_wdt();

const uint8_t init_address[] = "init_address";
mstr_sens_ids::M_S_IDS ms_ids = mstr_sens_ids::read();
uint32_t data_to_send[2];
RF24 radio = RF24(CE, CS);

DHT11 dht = DHT11(DHT11_PIN);
DHT11_DATA dht_data;

Timer timer(TIMER_UNITS::TIMER_MS);
volatile uint8_t timer_flag;

void setup() {
  if (!radio.begin()){
    Serial.println("Failed");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(10, 15);

  if (ms_ids.master == MASTER_DEFAULT){
    Serial.println("Asking for data address...");

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

  timer.start(10000);
  init_wdt();
  sei();
}

void loop() {
  wdt_reset();

  if (timer_flag){
    timer_flag = 0;

    dht_data = dht.read();
    uint32_t data_to_send[5];

    Serial.print("\n\nHumidity: ");
    Serial.println(dht_data.humidity);

    Serial.print("Temperature: ");
    Serial.println(dht_data.temperature);

    data_to_send[0] = (uint32_t)(ms_ids.sensor);
    data_to_send[1] = (uint32_t)(dht_data.humidity * 100);
    data_to_send[2] = (uint32_t)(dht_data.temperature * 100);
    data_to_send[3] = 0;
    data_to_send[4] = 0;

    radio.write(data_to_send, sizeof(data_to_send));
  }

}

void init_wdt(){
  WDTCSR |= (1U << 5);
  WDTCSR |= (1U << 3);
}

ISR(TIMER1_COMPA_vect){
  timer.inc_passed();

  if (timer.comp_passed_delay()){
    timer_flag = 1;
  }
}
