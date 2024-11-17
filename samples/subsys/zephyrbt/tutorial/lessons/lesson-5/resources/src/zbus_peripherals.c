/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/led.h>
#include <zephyr/zbus/zbus.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(peripherals, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "zbus_peripherals.h"

ZBUS_CHAN_DEFINE(peripherals_channel, struct tutorial_peripherals, NULL, NULL,
		 ZBUS_OBSERVERS(peripherals_thread_subscriber), ZBUS_MSG_INIT(0));
ZBUS_SUBSCRIBER_DEFINE(peripherals_thread_subscriber, 4);

static const char *peripherals_leds_state[] = {
	"Undefined",
	"off",
	"on",
	"blink",
};

static int _set_led(struct pwm_led_state *current_state, struct pwm_led_state *new_state,
		    uint32_t index, const char *name)
{
	static const struct device *const dev = DEVICE_DT_GET(DT_NODELABEL(pwmleds));
	int ret = 0;

	if (memcmp(current_state, new_state, sizeof(struct pwm_led_state)) == 0) {
		return ret;
	}

	LOG_INF("set %s %s", name, peripherals_leds_state[new_state->state]);

	switch (new_state->state) {
	case LEDS_STATES_OFF:
		ret = led_off(dev, index);
		break;
	case LEDS_STATES_ON:
		ret = led_on(dev, index);
		break;
	case LEDS_STATES_PWM:
		ret = led_blink(dev, index, new_state->high_period, new_state->low_period);
		break;
	}

	if (ret) {
		LOG_ERR("fail to set %s %s", name, peripherals_leds_state[new_state->state]);
		return ret;
	}

	memcpy(current_state, new_state, sizeof(struct pwm_led_state));

	return ret;
}

#define set_led(new_state, dt_name)                                                                \
	static struct pwm_led_state current_##dt_name;                                             \
	static const uint32_t index_##dt_name = DT_NODE_CHILD_IDX(DT_NODELABEL(dt_name));          \
	_set_led(&current_##dt_name, new_state, index_##dt_name, STRINGIFY(dt_name))

static void peripherals_thread(void)
{
	const struct zbus_channel *channel;
	struct tutorial_peripherals peripherals;

	LOG_DBG("peripherals_thread");

	while (!zbus_sub_wait(&peripherals_thread_subscriber, &channel, K_FOREVER)) {
		zbus_chan_read(&peripherals_channel, &peripherals, K_FOREVER);

		if (peripherals.led_green.state) {
			set_led(&peripherals.led_green, led_green);
		}
		if (peripherals.led_red.state) {
			set_led(&peripherals.led_red, led_red);
		}
	}
}

K_THREAD_DEFINE(peripherals_thread_id, 2048, peripherals_thread, NULL, NULL, NULL,
		(CONFIG_MAIN_THREAD_PRIORITY + 4), 0, 0);
