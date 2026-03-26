/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/random/random.h>
#include <zephyr/zephyrbt/zephyrbt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(port_alias, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "port_alias.h"

enum zephyrbt_child_status zephyrbt_action_write_value_init(struct zephyrbt_context *ctx,
							    struct zephyrbt_node *self)
{
	struct zephyrbt_blackboard_item *item = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_WRITE_VALUE_ATTRIBUTE_VALUE_PORT);
	if (!item) {
		LOG_ERR("write_value [%d]: blackboard item not found", self->index);
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	uint32_t counter = sys_rand32_get();

	/*
	 * Set initial value
	 */
	item->item = (zephyrbt_node_context_t)counter;

	/*
	 * When using CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT=y it is possible
	 * to initialize the blackboard item index to avoid runtime search.
	 *
	 * Use self->ctx to point to blackboard item.
	 */
	self->ctx = &item->item;

	LOG_INF("initial value [%d]: wrote %u", self->index, counter);

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_write_value(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self)
{
	/*
	 * The zephyrbt_action_FOO_init already makes the checks to ensure that
	 * the value available. This means that at runtime action can ignore
	 * checks to speed up the tree.
	 */
	uint32_t *counter = (uint32_t *)self->ctx;

	(*counter)++;

	LOG_INF("write_value [%d]: wrote %u", self->index, *counter);

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_read_value_init(struct zephyrbt_context *ctx,
							   struct zephyrbt_node *self)
{
	struct zephyrbt_blackboard_item *item = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_READ_VALUE_ATTRIBUTE_VALUE_PORT);
	if (!item) {
		LOG_ERR("write_value [%d]: blackboard item not found", self->index);
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	self->ctx = &item->item;

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_read_value(struct zephyrbt_context *ctx,
						      struct zephyrbt_node *self)
{
	uint32_t *counter = (uint32_t *)self->ctx;

	LOG_INF("read_value [%d]: read %u", self->index, *counter);

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
