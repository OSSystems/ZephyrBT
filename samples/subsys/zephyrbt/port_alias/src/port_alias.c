/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/zephyrbt/zephyrbt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(port_alias, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "port_alias.h"

static int write_counter;

enum zephyrbt_child_status zephyrbt_action_write_value(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self)
{
	struct zephyrbt_blackboard_item *item = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_WRITE_VALUE_ATTRIBUTE_VALUE_PORT);
	if (!item) {
		LOG_ERR("write_value [%d]: blackboard item not found", self->index);
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	write_counter++;
	item->item = (zephyrbt_node_context_t)(intptr_t)write_counter;
	LOG_INF("write_value [%d]: wrote %d", self->index, write_counter);

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_read_value(struct zephyrbt_context *ctx,
						      struct zephyrbt_node *self)
{
	struct zephyrbt_blackboard_item *item = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_READ_VALUE_ATTRIBUTE_VALUE_PORT);
	if (!item) {
		LOG_ERR("read_value [%d]: blackboard item not found", self->index);
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	int value = (int)(intptr_t)item->item;
	LOG_INF("read_value [%d]: read %d", self->index, value);

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
