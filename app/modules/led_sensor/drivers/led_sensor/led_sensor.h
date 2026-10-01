/* SPDX-License-Identifier: Apache-2.0 */

#ifndef LED_SENSOR_H_
#define LED_SENSOR_H_

#include <zephyr/device.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LED_SENSOR_DT_COMPAT zephyr_led_sensor

int led_sensor_set_enabled(const struct device *dev, bool enabled);

#ifdef __cplusplus
}
#endif

#endif
