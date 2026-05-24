#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include "led_sensor.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led0)

/* ── Task 1: fetch subcommand ── */
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = DEVICE_DT_GET(LED_NODE);

	if (!device_is_ready(dev)) {
		shell_error(sh, "Device not ready");
		return -ENODEV;
	}

	int ret = sensor_sample_fetch(dev);

	if (ret < 0) {
		shell_error(sh, "sensor_sample_fetch failed: %d", ret);
		return ret;
	}

	shell_print(sh, "Fetch OK — LED turned ON");
	return 0;
}

/* ── Task 1: read subcommand ── */
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = DEVICE_DT_GET(LED_NODE);
	struct sensor_value val;

	if (!device_is_ready(dev)) {
		shell_error(sh, "Device not ready");
		return -ENODEV;
	}

	int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);

	if (ret < 0) {
		shell_error(sh, "sensor_channel_get failed: %d", ret);
		return ret;
	}

	shell_print(sh, "LED state: %d", val.val1);
	return 0;
}

/* ── Task 1: info subcommand ── */
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = DEVICE_DT_GET(LED_NODE);

	shell_print(sh, "Device name:  %s", dev->name);
	shell_print(sh, "Device ready: %s", device_is_ready(dev) ? "yes" : "no");
	return 0;
}

/* ── Task 2: set subcommand ── */
static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = DEVICE_DT_GET(LED_NODE);

	if (argc < 2) {
		shell_error(sh, "Usage: sensor set <value>");
		return -EINVAL;
	}

	char *endptr;
	long val = strtol(argv[1], &endptr, 10);

	if (*endptr != '\0' || val < 0 || val > UINT32_MAX) {
		shell_error(sh, "Invalid value: %s (must be a positive integer)", argv[1]);
		return -EINVAL;
	}

	if (!device_is_ready(dev)) {
		shell_error(sh, "Device not ready");
		return -ENODEV;
	}

	int ret = led_sensor_set_blink_count(dev, (uint32_t)val);

	if (ret < 0) {
		shell_error(sh, "set_blink_count failed: %d", ret);
		return ret;
	}

	shell_print(sh, "Blink count set to: %ld", val);
	return 0;
}

/* ── Register shell commands ── */
SHELL_STATIC_SUBCMD_SET_CREATE(sensor_cmds,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch() — turns LED ON",  cmd_sensor_fetch),
	SHELL_CMD(read,  NULL, "Call sensor_channel_get() — turns LED OFF", cmd_sensor_read),
	SHELL_CMD(info,  NULL, "Print device name and ready state",          cmd_sensor_info),
	SHELL_CMD_ARG(set, NULL, "Set blink count <value>", cmd_sensor_set, 2, 0),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_cmds, "LED sensor commands", NULL);

int main(void)
{
	LOG_INF("LED sensor shell app started");
	LOG_INF("Try: sensor fetch / sensor read / sensor info / sensor set <n>");
	return 0;
}