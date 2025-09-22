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

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&button_up)) {
		printk("Error: button_up device %s is not ready\n",
		       button_up.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button_up, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button_up.port->name, button_up.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_up, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button_up.port->name, button_up.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button_up.pin));
	gpio_add_callback(button_up.port, &button_cb_data);
	printk("Set up button_up at %s pin %d\n", button_up.port->name, button_up.pin);

	if (!gpio_is_ready_dt(&button_left)) {
		printk("Error: button_left device %s is not ready\n",
		       button_left.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button_left, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button_left.port->name, button_left.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_left, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button_left.port->name, button_left.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button_left.pin));
	gpio_add_callback(button_left.port, &button_cb_data);
	printk("Set up button_left at %s pin %d\n", button_left.port->name, button_left.pin);

	if (!gpio_is_ready_dt(&button_right)) {
		printk("Error: button_right device %s is not ready\n",
		       button_right.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button_right, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button_right.port->name, button_right.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_right, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button_right.port->name, button_right.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button_right.pin));
	gpio_add_callback(button_right.port, &button_cb_data);
	printk("Set up button_right at %s pin %d\n", button_right.port->name, button_right.pin);


	ret = gpio_pin_configure_dt(&button_down, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button_down.port->name, button_down.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_down, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button_down.port->name, button_down.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button_down.pin));
	gpio_add_callback(button_down.port, &button_cb_data);
	printk("Set up button_down at %s pin %d\n", button_down.port->name, button_down.pin);

	if (led0.port && !gpio_is_ready_dt(&led0)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led0.port->name);
		led0.port = NULL;
	}
	if (led1.port && !gpio_is_ready_dt(&led1)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led1.port->name);
		led1.port = NULL;
	}
	if (led2.port && !gpio_is_ready_dt(&led2)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led2.port->name);
		led2.port = NULL;
	}
	if (led3.port && !gpio_is_ready_dt(&led3)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led3.port->name);
		led3.port = NULL;
	}

	if (led0.port) {
		ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led0.port->name, led0.pin);
			led0.port = NULL;
		} else {
			printk("Set up LED0 at %s pin %d\n", led0.port->name, led0.pin);
		}
	}
	if (led1.port) {
		ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led1.port->name, led1.pin);
			led1.port = NULL;
		} else {
			printk("Set up LED1 at %s pin %d\n", led1.port->name, led1.pin);
		}
	}
	if (led2.port) {
		ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led2.port->name, led2.pin);
			led2.port = NULL;
		} else {
			printk("Set up LED2 at %s pin %d\n", led2.port->name, led2.pin);
		}
	}
	if (led3.port) {
		ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led3.port->name, led3.pin);
			led3.port = NULL;
		} else {
			printk("Set up LED3 at %s pin %d\n", led3.port->name, led3.pin);
		}
	}

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
