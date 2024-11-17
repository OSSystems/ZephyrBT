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

struct zephyrbt_action_grab_beer_context {
	int counter;
};

enum zephyrbt_child_status zephyrbt_action_grab_beer_init(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
	LOG_DBG("zephyrbt_action_grab_beer_init stub function");

	struct zephyrbt_action_grab_beer_context *grab_beeer_ctx;

	grab_beeer_ctx = k_malloc(sizeof(struct zephyrbt_action_grab_beer_context));
	self->ctx = grab_beeer_ctx;

	if (grab_beeer_ctx == NULL) {
		LOG_ERR("Context can not be allocate. Need more memory!!");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	memset(grab_beeer_ctx, 0, sizeof(struct zephyrbt_action_grab_beer_context));

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_grab_beer(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self)
{
	struct zephyrbt_action_grab_beer_context *grab_beeer_ctx;
	grab_beeer_ctx = (struct zephyrbt_action_grab_beer_context *)self->ctx;

	++grab_beeer_ctx->counter;

	LOG_DBG("\nGrab Beer try: %d\n", grab_beeer_ctx->counter);
	k_msleep(1000);

	if (grab_beeer_ctx->counter < 3) {
		return ZEPHYRBT_CHILD_SUCCESS_STATUS;
	}

	return ZEPHYRBT_CHILD_FAILURE_STATUS;
}
