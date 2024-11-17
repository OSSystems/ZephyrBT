/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZBUS_PERIPHERALS_H
#define ZBUS_PERIPHERALS_H

enum led_state {
	LEDS_STATES_OFF = 1,
	LEDS_STATES_ON,
	LEDS_STATES_PWM,
} __attribute__((__packed__));

struct pwm_led_state {
	enum led_state state;
	uint32_t high_period;
	uint32_t low_period;
} __attribute__((__packed__));

struct tutorial_peripherals {
	struct pwm_led_state led_green;
	struct pwm_led_state led_red;
};

#define ZBUS_PERIPHERAL_WRITE(...)								\
	zbus_chan_pub(&peripherals_channel, ((struct tutorial_peripherals[]){{__VA_ARGS__}}),	\
		      K_NO_WAIT)

#endif
