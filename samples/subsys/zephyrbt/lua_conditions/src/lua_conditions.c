/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/zephyrbt/zephyrbt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lua_conditions, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "lua_conditions.h"

enum zephyrbt_child_status zephyrbt_action_increment_init(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
	struct zephyrbt_blackboard_item *item =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_INCREMENT_ATTRIBUTE_COUNTER);

	self->ctx = item;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_increment(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self)
{
	struct zephyrbt_blackboard_item *item = (struct zephyrbt_blackboard_item *)self->ctx;
	intptr_t val = (intptr_t)item->item;

	val++;
	item->item = (zephyrbt_node_context_t)val;
	LOG_INF("increment: counter=%d", (int)val);
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_condition_check_value_init(struct zephyrbt_context *ctx,
							       struct zephyrbt_node *self)
{
	LOG_DBG("check_value_init");
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_condition_check_value(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
	LOG_INF("check_value: ticked (odd counter)");
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_guarded_action_init(struct zephyrbt_context *ctx,
							       struct zephyrbt_node *self)
{
	LOG_DBG("guarded_action_init");
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_guarded_action(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
	LOG_INF("guarded_action: executed");
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
