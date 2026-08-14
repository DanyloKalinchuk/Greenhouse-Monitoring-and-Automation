#include "ble_comm.h"

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define ENV_SENSING_SERV_UUID 0x181A

LOG_MODULE_REGISTER(ble_comm, LOG_LEVEL_ERR);

static const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
    BT_LE_ADV_OPT_SCANNABLE,
    320,
    320,
    NULL
);

static struct ServiceData service_data = {
    .uuid = ENV_SENSING_SERV_UUID,
    .data = {
        .temperature = 0,
        .humidity = 0,
        .co2 = 0,
        .soil_moisture = 0,
    }
};

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(ENV_SENSING_SERV_UUID)),
    BT_DATA(BT_DATA_SVC_DATA16, (unsigned char *)&service_data, sizeof(service_data)),
};

static struct bt_data sd[] = {
    
};

uint8_t ble_init(void){
    int err;

    err = bt_enable(NULL);
    if (err){
        LOG_ERR("bt_enbale failed, err: %d", err);
        return err;
    }

    return 0;
}

uint8_t ble_start(struct SENS_DATA *adv_data){
    service_data.data = *adv_data;

    return bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
}

uint8_t ble_stop(void){
    return bt_le_adv_stop();
}

uint8_t ble_update(struct SENS_DATA *adv_data){
    service_data.data = *adv_data;

    return bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);
}
