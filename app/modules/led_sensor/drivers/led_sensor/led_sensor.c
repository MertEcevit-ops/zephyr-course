/* SPDX-License-Identifier: Apache-2.0 */

#define DT_DRV_COMPAT zephyr_led_sensor

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led_sensor, CONFIG_SENSOR_LOG_LEVEL);

struct led_sensor_config {
	struct gpio_dt_spec led;
};

struct led_sensor_data {
	int32_t value;
	bool enabled;
};

int led_sensor_set_enabled(const struct device *dev, bool enabled)
{
	const struct led_sensor_config *config = dev->config;
	struct led_sensor_data *data = dev->data;

	if (!device_is_ready(dev) || !gpio_is_ready_dt(&config->led)) {
		return -ENODEV;
	}

	data->enabled = enabled;
	if (gpio_pin_set_dt(&config->led, enabled ? 1 : 0) < 0) {
		return -EIO;
	}

	return 0;
}

static int led_sensor_sample_fetch(const struct device *dev,
					  enum sensor_channel chan)
{
	const struct led_sensor_config *config = dev->config;
	struct led_sensor_data *data = dev->data;

	if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_LIGHT) {
		return -ENOTSUP;
	}

	if (!gpio_is_ready_dt(&config->led)) {
		return -ENODEV;
	}
	if (!data->enabled) {
		return -EACCES;
	}

	if (gpio_pin_set_dt(&config->led, 1) < 0) {
		return -EIO;
	}

	data->value = 1;
	return 0;
}

static int led_sensor_channel_get(const struct device *dev,
					  enum sensor_channel chan,
					  struct sensor_value *value)
{
	const struct led_sensor_config *config = dev->config;
	struct led_sensor_data *data = dev->data;

	if (chan != SENSOR_CHAN_LIGHT || value == NULL) {
		return -ENOTSUP;
	}

	if (gpio_pin_set_dt(&config->led, 0) < 0) {
		return -EIO;
	}

	data->value = 0;
	value->val1 = data->value;
	value->val2 = 0;
	return 0;
}

static int led_sensor_init(const struct device *dev)
{
	const struct led_sensor_config *config = dev->config;
	struct led_sensor_data *data = dev->data;

	if (!gpio_is_ready_dt(&config->led)) {
		return -ENODEV;
	}

	data->enabled = true;
	return gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
}

static DEVICE_API(sensor, led_sensor_api) = {
	.sample_fetch = led_sensor_sample_fetch,
	.channel_get = led_sensor_channel_get,
};

#define LED_SENSOR_DEFINE(inst) \
	static struct led_sensor_data led_sensor_data_##inst; \
	static const struct led_sensor_config led_sensor_config_##inst = { \
		.led = GPIO_DT_SPEC_INST_GET(inst, gpios), \
	}; \
	DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL, \
		&led_sensor_data_##inst, &led_sensor_config_##inst, \
		POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &led_sensor_api)

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)
