/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt_decorator_delay, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

struct zephyrbt_decorator_delay_context {
	struct zephyrbt_blackboard_item *msec;
};

enum zephyrbt_child_status zephyrbt_decorator_delay_init(struct zephyrbt_context *ctx,
							 struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("init: %s", self->name);
#endif

	struct zephyrbt_decorator_delay_context *delay;

	delay = k_malloc(sizeof(struct zephyrbt_decorator_delay_context));
	self->ctx = delay;

	if (delay == NULL) {
		LOG_ERR("Context can not be allocate.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	memset(delay, 0, sizeof(struct zephyrbt_decorator_delay_context));

	delay->msec =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_DELAY_ATTRIBUTE_DELAY_MSEC);

	if (delay->msec == NULL) {
		LOG_ERR("Invalid delay msec value.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_decorator_delay(struct zephyrbt_context *ctx,
						    struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	struct zephyrbt_decorator_delay_context *delay;
	delay = (struct zephyrbt_decorator_delay_context *)self->ctx;

	if (delay == NULL) {
		LOG_ERR("Undefined behaviour on zephyrbt_decorator_delay.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	k_msleep((uintptr_t)delay->msec->item);

	return zephyrbt_evaluate(ctx, zephyrbt_get_node(ctx, self->child));
}
