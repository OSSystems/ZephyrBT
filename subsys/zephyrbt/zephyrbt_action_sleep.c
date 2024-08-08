/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt_action_sleep, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

struct zephyrbt_action_sleep_context {
	struct zephyrbt_blackboard_item *msec;
};

enum zephyrbt_child_status zephyrbt_action_sleep_init(struct zephyrbt_context *ctx,
						      struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("init: %s", self->name);
#endif

	struct zephyrbt_action_sleep_context *sleep;

	sleep = k_malloc(sizeof(struct zephyrbt_action_sleep_context));
	self->ctx = sleep;

	if (sleep == NULL) {
		LOG_ERR("Context can not be allocate.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	memset(sleep, 0, sizeof(struct zephyrbt_action_sleep_context));

	sleep->msec = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SLEEP_ATTRIBUTE_MSEC);

	if (sleep->msec == NULL) {
		LOG_ERR("Invalid sleep msec value.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_sleep(struct zephyrbt_context *ctx,
						 struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	struct zephyrbt_action_sleep_context *sleep;
	sleep = (struct zephyrbt_action_sleep_context *)self->ctx;

	if (sleep == NULL) {
		LOG_ERR("Undefined behaviour on zephyrbt_action_sleep.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	k_msleep((uintptr_t)sleep->msec->item);

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
