/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * NOTE: If you are looking into an implementation of button events with
 * debouncing, check out `input` subsystem and `samples/subsys/input/input_dump`
 * example instead.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

#define SLEEP_TIME_MS	1

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
//#define SW0_NODE	DT_ALIAS(sw0)
#define J_UP	DT_ALIAS(joy_up)
#define J_LEFT	DT_ALIAS(joy_left)
#define J_RIGHT	DT_ALIAS(joy_right)
#define J_DOWN	DT_ALIAS(joy_down)
#if !DT_NODE_HAS_STATUS_OKAY(J_UP)
#error "Unsupported board: joy_up devicetree alias is not defined"
#endif
#if !DT_NODE_HAS_STATUS_OKAY(J_LEFT)
#error "Unsupported board: joy_left devicetree alias is not defined"
#endif
#if !DT_NODE_HAS_STATUS_OKAY(J_RIGHT)
#error "Unsupported board: joy_right devicetree alias is not defined"
#endif
#if !DT_NODE_HAS_STATUS_OKAY(J_DOWN)
#error "Unsupported board: joy_down devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button_up    = GPIO_DT_SPEC_GET_OR(J_UP, gpios, {0});
static const struct gpio_dt_spec button_left  = GPIO_DT_SPEC_GET_OR(J_LEFT, gpios, {0});
static const struct gpio_dt_spec button_right = GPIO_DT_SPEC_GET_OR(J_RIGHT, gpios, {0});
static const struct gpio_dt_spec button_down  = GPIO_DT_SPEC_GET_OR(J_DOWN, gpios, {0});

static struct gpio_callback button_cb_data;

/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on the LED whenever the button is pressed.
 */
static struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,{0});
static struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios,{0});
static struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios,{0});
static struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led3), gpios,{0});

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int init_button_and_led(const struct gpio_dt_spec *button, struct gpio_dt_spec *led)
{
	if (!gpio_is_ready_dt(button)) {
		printk("Error: button_up device %s is not ready\n",
		       button->port->name);
		return -1;
	}

	int ret = gpio_pin_configure_dt(button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button->port->name, button->pin);
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button->port->name, button->pin);
		return -1;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button->pin));
	gpio_add_callback(button->port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button->port->name, button->pin);

	if (led->port && !gpio_is_ready_dt(led)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led->port->name);
		led->port = NULL;
	}

	if (led->port) {
		ret = gpio_pin_configure_dt(led, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led->port->name, led->pin);
			led->port = NULL;
		} else {
			printk("Set up LED at %s pin %d\n", led->port->name, led->pin);
		}
	}
	return 0;
}

int main(void)
{
	//int ret;

	init_button_and_led(&button_up,    &led0);
	init_button_and_led(&button_left,  &led1);
	init_button_and_led(&button_right, &led2);
	init_button_and_led(&button_down,  &led3);

	printk("Press the button\n");
	if (led0.port) {
		while (1) {
			/* If we have an LED, match its state to the button's. */
			int val0 = gpio_pin_get_dt(&button_up);
			if (val0 >= 0) {
				gpio_pin_set_dt(&led0, val0);
			}
			int val1 = gpio_pin_get_dt(&button_left);
			if (val1 >= 0) {
				gpio_pin_set_dt(&led1, val1);
			}
			int val2 = gpio_pin_get_dt(&button_right);
			if (val2 >= 0) {
				gpio_pin_set_dt(&led2, val2);
			}
			int val3 = gpio_pin_get_dt(&button_down);
			if (val3 >= 0) {
				gpio_pin_set_dt(&led3, val3);
			}
			k_msleep(SLEEP_TIME_MS);
		}
	}
	return 0;
}
