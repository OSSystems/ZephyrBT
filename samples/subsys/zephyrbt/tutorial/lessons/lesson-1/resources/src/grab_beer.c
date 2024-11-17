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
LOG_MODULE_REGISTER(grab_beer, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

enum zephyrbt_child_status zephyrbt_action_grab_beer(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self)
{
	static int count = 0;

	++count;

	LOG_DBG("\nGrab Beer try: %d\n", count);

	if (count < 3) {
		return ZEPHYRBT_CHILD_SUCCESS_STATUS;
	}

	return ZEPHYRBT_CHILD_FAILURE_STATUS;
}
