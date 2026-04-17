/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(scan, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "robot_patrol.h"
#include "robot_state.h"

/*
 * Scan animation phases (1 tick each):
 *  -1 IDLE:    Done, return SUCCESS (reset for next)
 *   0 ARRIVED: Show R + "Arrived at waypoint"
 *   1 SEARCH:  Show ? + "Searching..."
 *   2 FOUND:   Show ! + "Found N targets!" (do scan)
 *   3 HOLD:    Show ! + "Heading home..."
 */
#define SCAN_IDLE    -1
#define SCAN_ARRIVED 0
#define SCAN_SEARCH  1
#define SCAN_FOUND   2
#define SCAN_HOLD    3

struct scan_ctx {
	struct zephyrbt_blackboard_item *pos_x;
	struct zephyrbt_blackboard_item *pos_y;
	struct zephyrbt_blackboard_item *battery;
	struct zephyrbt_blackboard_item *scan_result;
	struct zephyrbt_blackboard_item *targets_found;
	struct zephyrbt_blackboard_item *scans_completed;
	struct zephyrbt_blackboard_item *mission_progress;
	int phase;
	int found;
	int last_px;
	int last_py;
};

enum zephyrbt_child_status zephyrbt_action_perform_scan_init(struct zephyrbt_context *ctx,
							     struct zephyrbt_node *self)
{
	struct scan_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->pos_x =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_PERFORM_SCAN_ATTRIBUTE_POS_X);
	c->pos_y =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_PERFORM_SCAN_ATTRIBUTE_POS_Y);
	c->battery = zephyrbt_search_blackboard(ctx, self->index,
						ZEPHYRBT_PERFORM_SCAN_ATTRIBUTE_BATTERY);
	c->scan_result = zephyrbt_search_blackboard(ctx, self->index,
						    ZEPHYRBT_PERFORM_SCAN_ATTRIBUTE_SCAN_RESULT);
	c->targets_found = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_PERFORM_SCAN_ATTRIBUTE_TARGETS_FOUND);
	c->scans_completed = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_PERFORM_SCAN_ATTRIBUTE_SCANS_COMPLETED);
	c->mission_progress = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_PERFORM_SCAN_ATTRIBUTE_MISSION_PROGRESS);
	c->phase = SCAN_ARRIVED;
	c->found = 0;
	c->last_px = -1;
	c->last_py = -1;

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_perform_scan(struct zephyrbt_context *ctx,
							struct zephyrbt_node *self)
{
	struct scan_ctx *c = (struct scan_ctx *)self->ctx;
	struct robot_state *s = robot_get_state();
	int px = (intptr_t)c->pos_x->item;
	int py = (intptr_t)c->pos_y->item;
	int bat = (intptr_t)c->battery->item;

	/* Position changed since last tick — a previous scan
	 * was aborted by _failureIf (e.g. battery drain) and
	 * the robot moved away. Restart scan from ARRIVED so
	 * the new waypoint gets a full scan. */
	if (px != c->last_px || py != c->last_py) {
		c->phase = SCAN_ARRIVED;
	}
	c->last_px = px;
	c->last_py = py;

	/* IDLE: scan complete, reset for next visit */
	if (c->phase == SCAN_IDLE) {
		display_set_scanning(0);
		display_update_status("Heading home");
		c->phase = SCAN_ARRIVED;
		return ZEPHYRBT_CHILD_SUCCESS_STATUS;
	}

	switch (c->phase) {
	case SCAN_ARRIVED:
		/* Show R at waypoint for one tick */
		display_set_scanning(0);
		display_update_status("Arrived at waypoint");
		display_render(s, px, py, bat, 0);
		c->phase = SCAN_SEARCH;
		return ZEPHYRBT_CHILD_RUNNING_STATUS;

	case SCAN_SEARCH:
		/* Show ? and do the actual scan */
		display_set_scanning(1);
		display_update_status("Searching...");

		c->found = 0;
		for (int dy = -SCAN_RADIUS; dy <= SCAN_RADIUS; dy++) {
			for (int dx = -SCAN_RADIUS; dx <= SCAN_RADIUS; dx++) {
				int sx = px + dx;
				int sy = py + dy;

				if (sx < 0 || sx >= GRID_SIZE || sy < 0 || sy >= GRID_SIZE) {
					continue;
				}
				if (s->grid[sy][sx] == CELL_TARGET) {
					c->found++;
				}
			}
		}

		display_render(s, px, py, bat, 0);
		c->phase = SCAN_FOUND;
		return ZEPHYRBT_CHILD_RUNNING_STATUS;

	case SCAN_FOUND: {
		/* Show ! and report results */
		char msg[48];

		display_set_scanning(2);
		snprintf(msg, sizeof(msg), "Found %d target%s!", c->found,
			 c->found != 1 ? "s" : "");
		display_update_status(msg);

		/* Drain battery */
		bat -= BATTERY_SCAN;
		if (bat < 0) {
			bat = 0;
		}
		c->battery->item = (zephyrbt_node_context_t)(intptr_t)bat;

		/* Update totals */
		intptr_t total = (intptr_t)c->targets_found->item;
		total += c->found;
		c->targets_found->item = (zephyrbt_node_context_t)total;
		c->scan_result->item = (zephyrbt_node_context_t)(intptr_t)c->found;
		s->total_targets = (int)total;

		display_update_scan(s->total_targets, c->found);
		display_set_visited(-1);
		display_render(s, px, py, bat, 0);

		/* Count the scan as completed as soon as the
		 * real work (battery drain + visited bitmap)
		 * has run. _onSuccess would only fire at
		 * SCAN_IDLE, two ticks later — if _failureIf
		 * aborts the node before then (e.g. battery
		 * just fell below critical), scans_completed
		 * would desync from the visited bitmap and the
		 * mission could never reach scans_completed==4. */
		intptr_t sc = (intptr_t)c->scans_completed->item + 1;
		c->scans_completed->item = (zephyrbt_node_context_t)sc;
		c->mission_progress->item =
			(zephyrbt_node_context_t)(intptr_t)((sc * 100) / NUM_WAYPOINTS);

		c->phase = SCAN_HOLD;
		return ZEPHYRBT_CHILD_RUNNING_STATUS;
	}

	case SCAN_HOLD:
		/* Hold ! for one more tick */
		display_set_scanning(2);
		display_update_status("Heading home...");
		display_render(s, px, py, bat, 0);
		c->phase = SCAN_IDLE;
		return ZEPHYRBT_CHILD_RUNNING_STATUS;
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
