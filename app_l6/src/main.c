#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "led_sensor.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led0)

int main(void)
{
	const struct device *dev = DEVICE_DT_GET(LED_NODE);
	struct sensor_value val;

	if (!device_is_ready(dev)) {
		LOG_ERR("LED sensor device not ready");
		return -ENODEV;
	}

	LOG_INF("LED sensor app started");

	/* Task 1: fetch turns LED ON */
	sensor_sample_fetch(dev);
	k_msleep(1000);

	/* Task 1: channel_get turns LED OFF */
	sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
	LOG_INF("Sensor value: %d", val.val1);
	k_msleep(1000);

	/* Task 2: custom extension — set blink count */
	led_sensor_set_blink_count(dev, 5);

	return 0;
}