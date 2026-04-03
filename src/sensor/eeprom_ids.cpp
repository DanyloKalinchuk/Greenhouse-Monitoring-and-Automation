#include "eeprom_ids.hpp"

#define SENSOR_ID (uint8_t *)(0)
#define MASTER_ID (uint8_t *)(2)

#define SENSOR_ID_VALUE (0x1U)

mstr_sens_ids::M_S_IDS mstr_sens_ids::read(){
  mstr_sens_ids::M_S_IDS res;

  res.master = eeprom_read_byte(MASTER_ID);
  res.sensor = eeprom_read_byte(SENSOR_ID);

  return res;
}

void mstr_sens_ids::update_master(uint8_t master_id){
  eeprom_update_byte(MASTER_ID, master_id);
}

void mstr_sens_ids::reset_master(){
  eeprom_update_byte(MASTER_ID, MASTER_DEFAULT);
}