/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/sys/util.h>
#include <zephyr/zephyrbt/zephyrbt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(global_ctx, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "tutorial.h"

enum zephyrbt_child_status zephyrbt_action_init_tree_init(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
	struct tutorial_context *global_ctx;

	global_ctx = k_malloc(sizeof(struct tutorial_context));
	self->ctx = global_ctx;

	if (global_ctx == NULL) {
		LOG_ERR("Context can not be allocate. Need more memory!!");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	memset(global_ctx, 0, sizeof(struct tutorial_context));

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
