/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt_decorator_timeout, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

enum zephyrbt_decorator_timeout_state {
	ZEPHYRBT_DECORATOR_TIMEOUT_STATE_IDLE,
	ZEPHYRBT_DECORATOR_TIMEOUT_STATE_RUNNING,
	ZEPHYRBT_DECORATOR_TIMEOUT_STATE_TIMEOUT,
};

struct zephyrbt_decorator_timeout_context {
	enum zephyrbt_decorator_timeout_state state;
	struct k_timer timer;
	struct zephyrbt_blackboard_item *msec;
};

static void zephyrbt_decorator_timeout_callback(struct k_timer *tick_tmr)
{
	struct zephyrbt_decorator_timeout_context *timeout = k_timer_user_data_get(tick_tmr);
	timeout->state = ZEPHYRBT_DECORATOR_TIMEOUT_STATE_TIMEOUT;
}

enum zephyrbt_child_status zephyrbt_decorator_timeout_init(struct zephyrbt_context *ctx,
							   struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("init: %s", self->name);
#endif

	struct zephyrbt_decorator_timeout_context *timeout;

	timeout = k_malloc(sizeof(struct zephyrbt_decorator_timeout_context));
	self->ctx = timeout;

	if (timeout == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	memset(timeout, 0, sizeof(struct zephyrbt_decorator_timeout_context));

	k_timer_init(&timeout->timer, zephyrbt_decorator_timeout_callback, NULL);
	k_timer_user_data_set(&timeout->timer, timeout);

	timeout->msec =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_TIMEOUT_ATTRIBUTE_MSEC);

	if (timeout->msec == NULL) {
		LOG_DBG("Invalid timeout msec value.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_decorator_timeout(struct zephyrbt_context *ctx,
						      struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	struct zephyrbt_decorator_timeout_context *timeout;
	timeout = (struct zephyrbt_decorator_timeout_context *)self->ctx;

	if (timeout == NULL) {
		LOG_DBG("Undefined behaviour on zephyrbt_decorator_timeout.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	if (timeout->state == ZEPHYRBT_DECORATOR_TIMEOUT_STATE_IDLE) {
		k_timer_start(&timeout->timer, K_MSEC((uintptr_t)timeout->msec->item), K_NO_WAIT);

		timeout->state = ZEPHYRBT_DECORATOR_TIMEOUT_STATE_RUNNING;
	}

	enum zephyrbt_child_status status =
		zephyrbt_evaluate(ctx, zephyrbt_get_node(ctx, self->child));

	if (timeout->state == ZEPHYRBT_DECORATOR_TIMEOUT_STATE_TIMEOUT) {
		timeout->state = ZEPHYRBT_DECORATOR_TIMEOUT_STATE_IDLE;

		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	if (status != ZEPHYRBT_CHILD_RUNNING_STATUS) {
		k_timer_stop(&timeout->timer);
		timeout->state = ZEPHYRBT_DECORATOR_TIMEOUT_STATE_IDLE;
	}

	return status;
}
