#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define LED_SENSOR_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_led_sensor)

static const struct device *const led_sensor = DEVICE_DT_GET(LED_SENSOR_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/**
 * @brief Main function
 *
 * This function initializes the LED and toggles it in a loop on STM32H723ZG Nucleo board.
 */
int main(void)
{
    struct sensor_value value;

    if (!device_is_ready(led_sensor)) return 0;

    while (1) {
        if (sensor_sample_fetch(led_sensor) < 0) return 0;
        LOG_INF("LED on");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        if (sensor_channel_get(led_sensor, SENSOR_CHAN_LIGHT, &value) < 0) return 0;
        LOG_INF("LED off");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
