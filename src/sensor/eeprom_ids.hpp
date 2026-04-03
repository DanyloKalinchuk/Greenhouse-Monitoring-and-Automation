#ifndef EEPROM_IDS
#define EEPROM_IDS

#include <avr/eeprom.h>
#include <stdint.h>

#define MASTER_DEFAULT (0x0U)

namespace mstr_sens_ids {
  struct M_S_IDS {
    uint8_t master;
    uint8_t sensor;
  };

  M_S_IDS read();
  void update_master(uint8_t master_id);
  void reset_master();
};

#endif