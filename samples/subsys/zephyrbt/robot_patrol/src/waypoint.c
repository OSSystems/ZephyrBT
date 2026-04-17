/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(waypoint, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "robot_patrol.h"
#include "robot_state.h"

struct select_wp_ctx {
	struct zephyrbt_blackboard_item *wp_x;
	struct zephyrbt_blackboard_item *wp_y;
	struct zephyrbt_blackboard_item *wp_index;
};

enum zephyrbt_child_status zephyrbt_action_select_waypoint_init(struct zephyrbt_context *ctx,
								struct zephyrbt_node *self)
{
	struct select_wp_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->wp_x = zephyrbt_search_blackboard(ctx, self->index,
					     ZEPHYRBT_SELECT_WAYPOINT_ATTRIBUTE_WP_X);
	c->wp_y = zephyrbt_search_blackboard(ctx, self->index,
					     ZEPHYRBT_SELECT_WAYPOINT_ATTRIBUTE_WP_Y);
	c->wp_index = zephyrbt_search_blackboard(ctx, self->index,
						 ZEPHYRBT_SELECT_WAYPOINT_ATTRIBUTE_WP_INDEX);

	/* Initialize: no waypoint selected */
	c->wp_x->item = (zephyrbt_node_context_t)(intptr_t)-1;
	c->wp_y->item = (zephyrbt_node_context_t)(intptr_t)-1;
	c->wp_index->item = (zephyrbt_node_context_t)0;

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_select_waypoint(struct zephyrbt_context *ctx,
							   struct zephyrbt_node *self)
{
	struct select_wp_ctx *c = (struct select_wp_ctx *)self->ctx;
	struct robot_state *s = robot_get_state();

	int idx = (intptr_t)c->wp_index->item;

	if (idx >= NUM_WAYPOINTS) {
		/* Gated by _skipIf="patrol_done(bb)": reaching
		 * this branch means a Lua condition is stale. */
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	int wx = s->waypoints[idx][0];
	int wy = s->waypoints[idx][1];

	c->wp_x->item = (zephyrbt_node_context_t)(intptr_t)wx;
	c->wp_y->item = (zephyrbt_node_context_t)(intptr_t)wy;

	c->wp_index->item = (zephyrbt_node_context_t)(intptr_t)(idx + 1);

	display_update_nav(idx, wx, wy, 0, 0);
	display_update_status("Heading to waypoint");

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
