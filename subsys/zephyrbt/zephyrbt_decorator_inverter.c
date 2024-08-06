/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/sys/util.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt_decorator_inverter, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

enum zephyrbt_child_status zephyrbt_decorator_inverter(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("init: %s", self->name);
#endif

	enum zephyrbt_child_status status =
		zephyrbt_evaluate(ctx, zephyrbt_get_node(ctx, self->child));

	if (status == ZEPHYRBT_CHILD_RUNNING_STATUS || status == ZEPHYRBT_CHILD_SKIP_STATUS) {
		return status;
	}

	return status == ZEPHYRBT_CHILD_SUCCESS_STATUS ? ZEPHYRBT_CHILD_FAILURE_STATUS
						       : ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
