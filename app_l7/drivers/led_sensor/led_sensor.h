#ifndef LED_SENSOR_H
#define LED_SENSOR_H

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

/* Custom extension API struct */
struct led_sensor_driver_api {
	sensor_sample_fetch_t sample_fetch;
	sensor_channel_get_t channel_get;
	int (*set_blink_count)(const struct device *dev, uint32_t count);
};

/* Custom extension inline wrapper — calls driver implementation */
static inline int led_sensor_set_blink_count(const struct device *dev, uint32_t count)
{
	const struct led_sensor_driver_api *api =
		(const struct led_sensor_driver_api *)dev->api;

	if (api->set_blink_count == NULL) {
		return -ENOSYS;
	}
	return api->set_blink_count(dev, count);
}

#endif /* LED_SENSOR_H */
