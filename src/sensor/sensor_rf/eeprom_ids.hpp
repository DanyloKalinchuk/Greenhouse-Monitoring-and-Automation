#ifndef EEPROM_IDS
#define EEPROM_IDS

#include <avr/eeprom.h>
#include <stdint.h>

#define MASTER_DEFAULT (0x0U) ///< Default value for the Master ID

namespace mstr_sens_ids {

  /**
   * \brief Structure that contains Master and Sensor IDs
   */
  struct M_S_IDS {
    uint8_t master;
    uint8_t sensor;
  };

  /**
   * \brief Reads Master and Sensor IDs in the EEPROM
   * 
   * \return Master and Sensor IDs as a M_S_IDS structure
   */
  M_S_IDS read();

  /**
   * \brief Sets Master ID to the provided value in the EEPROM
   * 
   * \param master_id New Master ID value
   */
  void update_master(uint8_t master_id);

  /**
   * \brief Sets Master ID to MASTER_DEFAULT in the EEPROM
   */
  void reset_master();
};

#endif