#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "led_sensor.h"

LOG_MODULE_REGISTER(led_sensor, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led0)

/* Dynamic data struct — modified by custom extension API */
struct led_sensor_data {
	bool led_on;
	uint32_t blink_count;
};

/* Device config */
struct led_sensor_config {
	struct gpio_dt_spec led;
};

/* Task 1: sensor_sample_fetch — turns LED ON */
static int led_sensor_sample_fetch(const struct device *dev,
				   enum sensor_channel chan)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;

	if (!gpio_is_ready_dt(&cfg->led)) {
		return -ENODEV;
	}

	gpio_pin_set_dt(&cfg->led, 1);
	data->led_on = true;
	LOG_INF("LED turned ON (fetch)");
	return 0;
}

/* Task 1: sensor_channel_get — turns LED OFF and returns state */
static int led_sensor_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;

	gpio_pin_set_dt(&cfg->led, 0);
	data->led_on = false;
	val->val1 = data->led_on ? 1 : 0;
	val->val2 = 0;
	LOG_INF("LED turned OFF (channel_get), state: %d", val->val1);
	return 0;
}

/* Task 2: custom extension — sets blink_count in dynamic data */
static int led_sensor_set_blink_count_impl(const struct device *dev, uint32_t count)
{
	struct led_sensor_data *data = dev->data;

	data->blink_count = count;
	LOG_INF("Blink count set to: %u", count);
	return 0;
}

static int led_sensor_init(const struct device *dev)
{
	const struct led_sensor_config *cfg = dev->config;

	if (!gpio_is_ready_dt(&cfg->led)) {
		LOG_ERR("LED GPIO not ready");
		return -ENODEV;
	}
	gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
	LOG_INF("LED sensor initialized");
	return 0;
}

static const struct led_sensor_driver_api led_sensor_api = {
	.sample_fetch   = led_sensor_sample_fetch,
	.channel_get    = led_sensor_channel_get,
	.set_blink_count = led_sensor_set_blink_count_impl,
};

static struct led_sensor_data led_sensor_data_0;

static const struct led_sensor_config led_sensor_config_0 = {
	.led = GPIO_DT_SPEC_GET(LED_NODE, gpios),
};

DEVICE_DT_DEFINE(LED_NODE,
		 led_sensor_init,
		 NULL,
		 &led_sensor_data_0,
		 &led_sensor_config_0,
		 POST_KERNEL,
		 CONFIG_SENSOR_INIT_PRIORITY,
		 &led_sensor_api);