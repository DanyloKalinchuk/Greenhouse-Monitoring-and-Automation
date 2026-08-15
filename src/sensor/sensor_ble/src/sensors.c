#include "sensors.h"

#define ADC_MAX_VAL 4095
#define ADC_TO_PERCENTAGE(raw_adc) ((raw_adc) * 100 / ADC_MAX_VAL)

#define PERIPH_PWR DT_NODELABEL(periph_pwr)

LOG_MODULE_REGISTER(sensors, LOG_LEVEL_ERR);

static const struct device *dht11_dev = DEVICE_DT_GET(DT_NODELABEL(dht11));
static struct sensor_value temp_val, hum_val;

static const struct adc_dt_spec co2_ch = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);
static uint32_t co2_buf;

struct adc_sequence co2_seq = {
        .buffer = &co2_buf,
        .buffer_size = sizeof(co2_buf),
};

static const struct adc_dt_spec moist_ch = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1);
static uint32_t moist_buf;

struct adc_sequence moist_seq = {
        .buffer = &moist_buf,
        .buffer_size = sizeof(moist_buf),
};

static const struct device *periph_pwr = DEVICE_DT_GET(PERIPH_PWR);

static uint16_t periph_prep_period;

uint8_t sensors_init(uint16_t periph_prep_s){
    int err;

    periph_prep_period = periph_prep_s;

    if (!device_is_ready(periph_pwr)){
        LOG_ERR("periph_pwr pin is not ready");
        return -1;
    }
    regulator_disable(periph_pwr);

    if (!device_is_ready(dht11_dev)){
        LOG_ERR("GPIO is not ready");
        return -1;
    }

    if (!adc_is_ready_dt(&co2_ch)){
        LOG_ERR("ADC co2 channel is not ready");
        return -1;
    }

    if (!adc_is_ready_dt(&moist_ch)){
        LOG_ERR("ADC soil moisture channel is not ready");
        return -1;
    }

    err = adc_channel_setup_dt(&co2_ch);
    if (err < 0){
        LOG_ERR("ADC co2 channel setup failed, err: %d", err);
        return err;
    }

    err = adc_channel_setup_dt(&moist_ch);
    if (err < 0){
        LOG_ERR("ADC soil moisture channel setup failed, err: %d", err);
        return err;
    }

    err = adc_sequence_init_dt(&co2_ch, &co2_seq);
    if (err < 0){
        LOG_ERR("Could not inititalize co2_seq");
        return err;
    }

    err = adc_sequence_init_dt(&moist_ch, &moist_seq);
    if (err < 0){
        LOG_ERR("Could not inititalize moist_seq");
        return err;
    }

    return 0;
}

uint8_t sensors_read(struct SENS_DATA *data){
    int err;

    regulator_enable(periph_pwr);

    k_sleep(K_SECONDS(periph_prep_period));

    err = sensor_sample_fetch(dht11_dev);
    if (err){
        LOG_ERR("sample_fetch failed, err: %d", err);
        return err;
    }

    err = sensor_channel_get(dht11_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp_val);
    if (err){
        LOG_ERR("Failed to get temperature channel, err: %d", err);
        return err;
    }

    err = sensor_channel_get(dht11_dev, SENSOR_CHAN_HUMIDITY, &hum_val);
    if (err){
        LOG_ERR("Failed to get humidity channel, err: %d", err);
        return err;
    }

    err = adc_read_dt(&co2_ch, &co2_seq);
    if (err < 0){
        LOG_ERR("Failed to read co2, err: %d", err);
        return err;
    }

    err = adc_read_dt(&moist_ch, &moist_seq);
    if (err < 0){
        LOG_ERR("Failed to read soil moisture, err: %d", err);
        return err;
    }

    regulator_disable(periph_pwr);

    data->temperature = temp_val.val1;
    data->humidity = hum_val.val1;
    data->co2 = ADC_TO_PERCENTAGE(co2_buf);
    data->soil_moisture = ADC_TO_PERCENTAGE(moist_buf);

    return 0;
}