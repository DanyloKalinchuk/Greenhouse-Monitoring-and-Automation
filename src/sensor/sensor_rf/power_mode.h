#ifndef POWER_MODE_H
#define POWER_MODE_H

#include <avr/io.h>
#include <stdint.h>

#define POWER_SAVE_MODE (0x3U << 1)

void pm_sleep(uint8_t power_mode);

#endif