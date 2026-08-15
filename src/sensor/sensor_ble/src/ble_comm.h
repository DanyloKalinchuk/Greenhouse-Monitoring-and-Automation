#ifndef BLE_COMM_H
#define BLE_COMM_H

#include <zephyr/logging/log.h>
#include <zephyr/dsp/types.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>

#include "sensors.h"

struct ServiceData {
    uint16_t uuid;
    struct SENS_DATA data;
};

/**
 * \brief Initializes BLE
 * 
 * \return 0 on success. otherwise error code
 */
uint8_t ble_init(void);

/**
 * \brief Starts BLE advertising
 * 
 * \param adv_data Pointer to the SENS_DATA structure that contains sensor data
 * \return 0 on success. otherwise error code
 */
uint8_t ble_start(struct SENS_DATA *adv_data);

/**
 * \brief Stops BLE advertising
 *
 * \return 0 on success. otherwise error code
 */
uint8_t ble_stop(void);

/**
 * \brief Updates BLE advertising data
 * 
 * \param adv_data Pointer to the SENS_DATA structure that contains sensor data
 * \return 0 on success. otherwise error code
 */
uint8_t ble_update(struct SENS_DATA *adv_data);

#endif