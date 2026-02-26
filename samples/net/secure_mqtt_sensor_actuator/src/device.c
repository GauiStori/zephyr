/*
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_device, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>
#include <zephyr/drivers/led.h>
#include <zephyr/random/random.h>

#include "device.h"

#define SENSOR_CHAN     SENSOR_CHAN_AMBIENT_TEMP
#define SENSOR_UNIT     "Celsius"

/* Devices */
//static const struct device *sensor = DEVICE_DT_GET_OR_NULL(DT_ALIAS(ambient_temp0));
static const struct device *const dev = DEVICE_DT_GET_ANY(bosch_bme280);
static const struct device *leds = DEVICE_DT_GET_OR_NULL(DT_INST(0, gpio_leds));

SENSOR_DT_READ_IODEV(iodev, DT_COMPAT_GET_ANY_STATUS_OKAY(bosch_bme280),
		{SENSOR_CHAN_AMBIENT_TEMP, 0},
		{SENSOR_CHAN_HUMIDITY, 0},
		{SENSOR_CHAN_PRESS, 0});

RTIO_DEFINE(ctx, 1, 1);


/* Command handlers */
static void led_on_handler(void)
{
	device_write_led(LED_USER, LED_ON);
}

static void led_off_handler(void)
{
	device_write_led(LED_USER, LED_OFF);
}

/* Supported device commands */
struct device_cmd device_commands[] = {
	{"led_on", led_on_handler},
	{"led_off", led_off_handler}
};

const size_t num_device_commands = ARRAY_SIZE(device_commands);

/* Command dispatcher */
void device_command_handler(uint8_t *command)
{
	for (int i = 0; i < num_device_commands; i++) {
		if (strcmp(command, device_commands[i].command) == 0) {
			LOG_INF("Executing device command: %s", device_commands[i].command);
			return device_commands[i].handler();
		}
	}
	LOG_ERR("Unknown command: %s", command);
}

int device_read_sensor(int *out)
{
	uint8_t buf[128];

	int rc = sensor_read(&iodev, &ctx, buf, 128);

	if (rc != 0) {
		printk("%s: sensor_read() failed: %d\n", dev->name, rc);
		return rc;
	}

	const struct sensor_decoder_api *decoder;

	rc = sensor_get_decoder(dev, &decoder);

	if (rc != 0) {
		printk("%s: sensor_get_decode() failed: %d\n", dev->name, rc);
		return rc;
	}

	uint32_t temp_fit = 0;
	struct sensor_q31_data temp_data = {0};

	decoder->decode(buf,
		(struct sensor_chan_spec) {SENSOR_CHAN_AMBIENT_TEMP, 0},
		&temp_fit, 1, &temp_data);

	uint32_t press_fit = 0;
	struct sensor_q31_data press_data = {0};

	decoder->decode(buf,
			(struct sensor_chan_spec) {SENSOR_CHAN_PRESS, 0},
			&press_fit, 1, &press_data);

	uint32_t hum_fit = 0;
	struct sensor_q31_data hum_data = {0};

	decoder->decode(buf,
			(struct sensor_chan_spec) {SENSOR_CHAN_HUMIDITY, 0},
			&hum_fit, 1, &hum_data);

	/*printk("temp: %s %d.%d; press: %s %d.%d; humidity: %s %d.%d\n",
		PRIq_arg(temp_data.readings[0].temperature, 6, temp_data.shift),
		PRIq_arg(press_data.readings[0].pressure, 6, press_data.shift),
		PRIq_arg(hum_data.readings[0].humidity, 6, hum_data.shift));
	printf("temp: %d; press:  %d; press:  %d\n",
	       temp_data.readings[0].temperature,
	       press_data.readings[0].pressure,
	       hum_data.readings[0].humidity);

	sample->unit = SENSOR_UNIT;
	sample->value = temp_data.shift; // sensor_value_to_double(&sensor_val);
	*/
	out[0] = temp_data.readings[0].temperature;
	out[1] = temp_data.shift;
	out[2] = press_data.readings[0].pressure;
	out[3] = press_data.shift;
	out[4] = hum_data.readings[0].humidity;
	out[5] = hum_data.shift;
	return rc;
}

int device_write_led(enum led_id led_idx, enum led_state state)
{
	int rc;

	switch (state) {
	case LED_OFF:
		if (leds == NULL) {
			LOG_INF("LED %d OFF", led_idx);
			break;
		}
		led_off(leds, led_idx);
		break;
	case LED_ON:
		if (leds == NULL) {
			LOG_INF("LED %d ON", led_idx);
			break;
		}
		led_on(leds, led_idx);
		break;
	default:
		LOG_ERR("Invalid LED state setting");
		rc = -EINVAL;
		break;
	}

	return rc;
}

bool devices_ready(void)
{
	bool rc = true;

	/* Check readiness only if a real sensor device is present */
	if (dev != NULL) {
		if (!device_is_ready(dev)) {
			LOG_ERR("Device %s is not ready", dev->name);
			rc = false;
		} else {
			LOG_INF("Device %s is ready", dev->name);
		}
	}

	if (leds != NULL) {
		if (!device_is_ready(leds)) {
			LOG_ERR("Device %s is not ready", leds->name);
			rc = false;
		} else {
			LOG_INF("Device %s is ready", leds->name);
		}
	}

	return rc;
}
