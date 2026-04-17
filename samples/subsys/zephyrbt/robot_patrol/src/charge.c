/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(charge, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "robot_patrol.h"
#include "robot_state.h"

/*
 * Charge node: recharge the battery when parked at the charger.
 *
 * Returns RUNNING while battery < BATTERY_START, SUCCESS once
 * full.  Callers typically gate it with
 *   _skipIf="bb.pos_x ~= bb.charger_x or
 *            bb.pos_y ~= bb.charger_y"
 * so obstacle emergencies (response != charger) no-op through.
 */

#define CHARGE_TICKS_PER_UNIT 2
#define CHARGE_UNIT           5

struct charge_ctx {
	struct zephyrbt_blackboard_item *battery;
	struct zephyrbt_blackboard_item *charging;
	int16_t charge_ticks;
};

enum zephyrbt_child_status zephyrbt_action_charge_init(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self)
{
	struct charge_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->battery =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_CHARGE_ATTRIBUTE_BATTERY);
	c->charging =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_CHARGE_ATTRIBUTE_CHARGING);

	c->charge_ticks = 0;

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_charge(struct zephyrbt_context *ctx,
						  struct zephyrbt_node *self)
{
	struct charge_ctx *c = (struct charge_ctx *)self->ctx;
	struct robot_state *rs = robot_get_state();

	intptr_t bat = (intptr_t)c->battery->item;

	if (bat >= BATTERY_START) {
		/* Fully charged — exit emergency loop */
		rs->charging = 0;
		c->charging->item = (zephyrbt_node_context_t)0;
		c->charge_ticks = 0;
		display_update_status("Resuming patrol...");
		return ZEPHYRBT_CHILD_SUCCESS_STATUS;
	}

	rs->charging = 1;
	c->charging->item = (zephyrbt_node_context_t)1;
	c->charge_ticks++;

	if (c->charge_ticks % CHARGE_TICKS_PER_UNIT == 0) {
		bat += CHARGE_UNIT;
		if (bat > BATTERY_START) {
			bat = BATTERY_START;
		}
		c->battery->item = (zephyrbt_node_context_t)bat;
	}

	display_update_status("Charging...");
	return ZEPHYRBT_CHILD_RUNNING_STATUS;
}
