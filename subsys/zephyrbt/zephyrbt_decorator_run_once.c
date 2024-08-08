/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt_decorator_runonce, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

enum zephyrbt_decorator_run_once_state {
	ZEPHYRBT_DECORATOR_RUN_ONCE_STATE_RUNNING,
	ZEPHYRBT_DECORATOR_RUN_ONCE_STATE_FINISHED,
};

struct zephyrbt_decorator_run_once_context {
	enum zephyrbt_decorator_run_once_state state;
	struct zephyrbt_blackboard_item *then_skip;
};

enum zephyrbt_child_status zephyrbt_decorator_run_once_init(struct zephyrbt_context *ctx,
							    struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("init: %s", self->name);
#endif

	struct zephyrbt_decorator_run_once_context *run_once;

	run_once = k_malloc(sizeof(struct zephyrbt_decorator_run_once_context));
	self->ctx = run_once;

	if (run_once == NULL) {
		LOG_ERR("Context can not be allocate.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	memset(run_once, 0, sizeof(struct zephyrbt_decorator_run_once_context));

	run_once->then_skip =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_RUN_ONCE_ATTRIBUTE_THEN_SKIP);

	if (run_once->then_skip == NULL) {
		LOG_DBG("Invalid then_skip value.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	run_once->state = ZEPHYRBT_DECORATOR_RUN_ONCE_STATE_RUNNING;

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_decorator_run_once(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	struct zephyrbt_decorator_run_once_context *run_once;
	run_once = (struct zephyrbt_decorator_run_once_context *)self->ctx;

	if (run_once->state == ZEPHYRBT_DECORATOR_RUN_ONCE_STATE_FINISHED) {
		return ZEPHYRBT_CHILD_SKIP_STATUS;
	}

	enum zephyrbt_child_status status =
		zephyrbt_evaluate(ctx, zephyrbt_get_node(ctx, self->child));

	if (status == ZEPHYRBT_CHILD_RUNNING_STATUS) {
		return ZEPHYRBT_CHILD_RUNNING_STATUS;
	}

	if (run_once->then_skip->item == (zephyrbt_node_context_t) true) {
		run_once->state = ZEPHYRBT_DECORATOR_RUN_ONCE_STATE_FINISHED;
	}

	return status;
}
