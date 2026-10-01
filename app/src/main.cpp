#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <cerrno>
#include <cstdlib>

#include "led_sensor.h"

#define LED_SENSOR_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_led_sensor)

static const struct device *const led_sensor = DEVICE_DT_GET(LED_SENSOR_NODE);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static int cmd_sensor_fetch(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (led_sensor_set_enabled(led_sensor, true) < 0 ||
        sensor_sample_fetch(led_sensor) < 0) {
        shell_error(shell, "sensor fetch failed");
        return -EIO;
    }

    shell_print(shell, "sensor fetched");
    return 0;
}

static int cmd_sensor_read(const struct shell *shell, size_t argc, char **argv)
{
    struct sensor_value value;

    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (sensor_channel_get(led_sensor, SENSOR_CHAN_LIGHT, &value) < 0) {
        shell_error(shell, "sensor read failed");
        return -EIO;
    }

    shell_print(shell, "value: %d.%06d", value.val1, value.val2);
    return 0;
}

static int cmd_sensor_info(const struct shell *shell, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(shell, "device: %s", led_sensor->name);
    shell_print(shell, "ready: %s", device_is_ready(led_sensor) ? "yes" : "no");
    return 0;
}

static int cmd_sensor_set(const struct shell *shell, size_t argc, char **argv)
{
    char *end;
    unsigned long enabled;

    ARG_UNUSED(argc);

    errno = 0;
    enabled = strtoul(argv[1], &end, 10);
    if (errno != 0 || end == argv[1] || *end != '\0' || enabled > 1U) {
        shell_error(shell, "value must be 0 (off) or 1 (on)");
        return -EINVAL;
    }

    if (led_sensor_set_enabled(led_sensor, enabled != 0U) < 0) {
        shell_error(shell, "failed to set sensor state");
        return -EIO;
    }

    shell_print(shell, "sensor %s", enabled != 0U ? "enabled" : "disabled");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_cmds,
    SHELL_CMD(fetch, NULL, "Fetch a sensor sample", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Read the light channel", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Show sensor device information", cmd_sensor_info),
    SHELL_CMD_ARG(set, NULL, "Set sensor state: 0=off, 1=on", cmd_sensor_set, 1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_cmds, "LED sensor commands", NULL);

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
        if (led_sensor_set_enabled(led_sensor, true) < 0) return 0;
        if (sensor_sample_fetch(led_sensor) < 0) return 0;
        LOG_INF("LED on");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        if (sensor_channel_get(led_sensor, SENSOR_CHAN_LIGHT, &value) < 0) return 0;
        if (led_sensor_set_enabled(led_sensor, false) < 0) return 0;
        LOG_INF("LED off");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
