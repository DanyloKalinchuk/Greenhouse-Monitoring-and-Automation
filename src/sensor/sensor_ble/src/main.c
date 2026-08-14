#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>

#include "sensors.h"
#include "ble_comm.h"

#define SENSOR_ID 1

#define SENSOR_DELAY 5 ///< Delay in minutes for the sensor data sending loop
#define SENSOR_DELAY_SEC SENSOR_DELAY * 60
#define SENSOR_DELAY_MSEC SENSOR_DELAY_SEC * 1000
#define PERIPH_PREP_PERIOD 40 ///< Time in seconds to power on peripherals before reading data
#define PERIPH_PREP_PERIOD_MSEC PERIPH_PREP_PERIOD * 1000
#define BT_ACTIVE_PERIOD 500 ///< Time in miliseconds the data is being advertised for
#define RESET_BTN_DELAY 10 ///< Time in seconds the reset button should be hold to reset sensor with setting master id to MASTER_DEFAULT

#define SENS_THREAD_STACK_SIZE 512
#define SENS_THREAD_PRIORITY 5

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_ACTIVE DT_NODELABEL(led_active)
#define RESET_BTN DT_NODELABEL(reset_btn)

static const struct gpio_dt_spec led_active = GPIO_DT_SPEC_GET(LED_ACTIVE, gpios);
static const struct gpio_dt_spec reset_btn = GPIO_DT_SPEC_GET(RESET_BTN, gpios);

static volatile uint8_t reset_btn_state = 0;
static struct gpio_callback reset_cb;
static void reset_isr(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins);

static struct k_work reset_work;
static void reset_work_handler(struct k_work *work);

static struct SENS_DATA data = {
        .sensor_id = SENSOR_ID,
};

void sens_thread_handling(void);
void reset_handling(struct k_timer *dummy);

K_TIMER_DEFINE(reset_timer, reset_handling, NULL);

int main(void){
        int err;

        LOG_INF("main started");

        if (!gpio_is_ready_dt(&led_active)){
            LOG_ERR("led_active is not ready");
            return -1;
        }
        gpio_pin_configure_dt(&led_active, (GPIO_OUTPUT | GPIO_PULL_UP | GPIO_ACTIVE_HIGH));

        if (!gpio_is_ready_dt(&reset_btn)){
                LOG_ERR("reset_btn is not ready");
                return -1;
        }
        gpio_pin_configure_dt(&reset_btn, (GPIO_INPUT | GPIO_PULL_DOWN));

        gpio_init_callback(&reset_cb, reset_isr, BIT(reset_btn.pin));
        gpio_add_callback_dt(&reset_btn, &reset_cb);
        gpio_pin_interrupt_configure_dt(&reset_btn, GPIO_INT_EDGE_BOTH);

        err = sensors_init(PERIPH_PREP_PERIOD);
        if (err){
                LOG_ERR("sensor_init() failed, err: %d", err);
                return -1;
        }

        err = ble_init();
        if (err){
                LOG_ERR("ble_adv_init failed, err: %d", err);
                return -1;
        }

        k_work_init(&reset_work, reset_work_handler);

        return 0;
}

K_THREAD_DEFINE(sensor_thread, SENS_THREAD_STACK_SIZE, sens_thread_handling, NULL, NULL, NULL, SENS_THREAD_PRIORITY, 0, 0);
void sens_thread_handling(void){
        int err;

        while (1){
                gpio_pin_set_dt(&led_active, 1);

                err = sensors_read(&data);
                if (err){
                        LOG_ERR("sensor_read() failed, err: %d", err);
                        return;
                }

                err = ble_start(&data);
                if (err){
                        LOG_ERR("ble_start failed, err: %d", err);
                        return;
                }

                k_sleep(K_MSEC(BT_ACTIVE_PERIOD));

                err = ble_stop();
                if (err){
                        LOG_ERR("ble_stop failed, err: %d", err);
                        return;
                }
                
                gpio_pin_set_dt(&led_active, 0);

                k_sleep(K_MSEC(SENSOR_DELAY_MSEC - PERIPH_PREP_PERIOD_MSEC - BT_ACTIVE_PERIOD));
        }
}

void reset_handling(struct k_timer *dummy){
        irq_lock();

        LOG_INF("Rebooting system...");

        gpio_pin_set_dt(&led_active, 0);

        sys_reboot(SYS_REBOOT_COLD);
        while (1){}
}

void reset_isr(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins){
        reset_btn_state = gpio_pin_get_dt(&reset_btn);
        
        if (reset_btn_state){
                k_timer_start(&reset_timer, K_SECONDS(RESET_BTN_DELAY), K_FOREVER);
                k_work_submit(&reset_work);
        }else{
                k_timer_stop(&reset_timer);
        }
}


void reset_work_handler(struct k_work *work){
        uint8_t led_state = gpio_pin_get_dt(&led_active);

        while (reset_btn_state){
                gpio_pin_toggle_dt(&led_active);
                k_sleep(K_MSEC(500));
        }

        gpio_pin_set_dt(&led_active, led_state);
}