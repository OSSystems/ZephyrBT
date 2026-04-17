/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(world, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "robot_patrol.h"
#include "robot_state.h"

static struct robot_state g_state;

struct robot_state *robot_get_state(void)
{
	return &g_state;
}

static void init_grid(struct robot_state *s)
{
	memset(s->grid, CELL_FREE, sizeof(s->grid));

	/* Obstacles */
	s->grid[0][2] = CELL_OBSTACLE;
	s->grid[1][2] = CELL_OBSTACLE;
	s->grid[2][4] = CELL_OBSTACLE;
	s->grid[3][4] = CELL_OBSTACLE;
	s->grid[5][1] = CELL_OBSTACLE;
	s->grid[5][2] = CELL_OBSTACLE;

	/* Scan targets (placed near waypoints) */
	s->grid[3][1] = CELL_TARGET;
	s->grid[0][5] = CELL_TARGET;
	s->grid[5][6] = CELL_TARGET;
	s->grid[7][4] = CELL_TARGET;
}

static void init_waypoints(struct robot_state *s)
{
	/* W1 */ s->waypoints[0][0] = 0;
	s->waypoints[0][1] = 3;
	/* W2 */ s->waypoints[1][0] = 6;
	s->waypoints[1][1] = 0;
	/* W3 */ s->waypoints[2][0] = 7;
	s->waypoints[2][1] = 5;
	/* W4 */ s->waypoints[3][0] = 5;
	s->waypoints[3][1] = 7;
}

static int is_reserved(int x, int y)
{
	return (x == CHARGER_X && y == CHARGER_Y) || (x == HOME_X && y == HOME_Y);
}

void robot_new_mission(struct robot_state *s)
{
	/* Clear grid */
	memset(s->grid, CELL_FREE, sizeof(s->grid));

	/* Place 6 random obstacles */
	int placed = 0;

	while (placed < 6) {
		int x = rand() % GRID_SIZE;
		int y = rand() % GRID_SIZE;

		if (s->grid[y][x] == CELL_FREE && !is_reserved(x, y)) {
			s->grid[y][x] = CELL_OBSTACLE;
			placed++;
		}
	}

	/* Place 4 random waypoints */
	for (int i = 0; i < NUM_WAYPOINTS; i++) {
		while (1) {
			int x = rand() % GRID_SIZE;
			int y = rand() % GRID_SIZE;

			if (s->grid[y][x] == CELL_FREE && !is_reserved(x, y)) {
				s->waypoints[i][0] = x;
				s->waypoints[i][1] = y;
				break;
			}
		}
	}

	/* Place targets near each waypoint so scans
	 * find them. Try adjacent cells first. */
	for (int i = 0; i < NUM_WAYPOINTS; i++) {
		int wx = s->waypoints[i][0];
		int wy = s->waypoints[i][1];
		int tries = 0;

		while (tries < 20) {
			int dx = (rand() % 3) - 1;
			int dy = (rand() % 3) - 1;
			int tx = wx + dx;
			int ty = wy + dy;

			if (tx >= 0 && tx < GRID_SIZE && ty >= 0 && ty < GRID_SIZE &&
			    s->grid[ty][tx] == CELL_FREE && !is_reserved(tx, ty)) {
				s->grid[ty][tx] = CELL_TARGET;
				break;
			}
			tries++;
		}
	}

	s->mission_num++;
	s->total_targets = 0;
	s->path.valid = false;
	display_set_visited(-2);
}

static int compute_obstacle_dist(struct robot_state *s, int px, int py)
{
	int min_dist = GRID_SIZE * 2;

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (s->grid[y][x] != CELL_OBSTACLE) {
				continue;
			}
			int dx = px - x;
			int dy = py - y;

			if (dx < 0) {
				dx = -dx;
			}
			if (dy < 0) {
				dy = -dy;
			}

			int dist = dx + dy;

			if (dist < min_dist) {
				min_dist = dist;
			}
		}
	}

	return min_dist;
}

#ifdef CONFIG_ROBOT_PATROL_TEST_MODE
#define BT_DELAY_MSEC 1
#else
#define BT_DELAY_MSEC 500
#endif

/* ---- Sense node ------------------------------------------------
 * Owns one-time world init (grid, RNG seed, blackboard constants)
 * and per-tick sensing (obstacle distance, tick counter, charging
 * flag mirror).  Does not render or decide respawns.
 * -------------------------------------------------------------- */

struct sense_ctx {
	struct zephyrbt_blackboard_item *battery;
	struct zephyrbt_blackboard_item *pos_x;
	struct zephyrbt_blackboard_item *pos_y;
	struct zephyrbt_blackboard_item *obstacle_dist;
	struct zephyrbt_blackboard_item *tick_count;
	struct zephyrbt_blackboard_item *delay_msec;
	struct zephyrbt_blackboard_item *charger_x;
	struct zephyrbt_blackboard_item *charger_y;
	struct zephyrbt_blackboard_item *critical_battery;
	struct zephyrbt_blackboard_item *min_clearance;
	struct zephyrbt_blackboard_item *safe_x;
	struct zephyrbt_blackboard_item *safe_y;
	struct zephyrbt_blackboard_item *home_x;
	struct zephyrbt_blackboard_item *home_y;
	struct zephyrbt_blackboard_item *idle_count;
	struct zephyrbt_blackboard_item *charging;
};

enum zephyrbt_child_status zephyrbt_action_sense_init(struct zephyrbt_context *ctx,
						      struct zephyrbt_node *self)
{
	struct robot_state *s = robot_get_state();
	struct sense_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	if (!s->initialized) {
		srand((unsigned)k_uptime_get());
		init_grid(s);
		init_waypoints(s);
		s->path.valid = false;
		s->mission_num = 1;
		s->initialized = true;
	}

	c->battery = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_BATTERY);
	c->pos_x = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_POS_X);
	c->pos_y = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_POS_Y);
	c->obstacle_dist = zephyrbt_search_blackboard(ctx, self->index,
						      ZEPHYRBT_SENSE_ATTRIBUTE_OBSTACLE_DIST);
	c->tick_count =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_TICK_COUNT);
	c->delay_msec =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_DELAY_MSEC);
	c->charger_x =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_CHARGER_X);
	c->charger_y =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_CHARGER_Y);
	c->critical_battery = zephyrbt_search_blackboard(ctx, self->index,
							 ZEPHYRBT_SENSE_ATTRIBUTE_CRITICAL_BATTERY);
	c->min_clearance = zephyrbt_search_blackboard(ctx, self->index,
						      ZEPHYRBT_SENSE_ATTRIBUTE_MIN_CLEARANCE);
	c->safe_x = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_SAFE_X);
	c->safe_y = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_SAFE_Y);
	c->home_x = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_HOME_X);
	c->home_y = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_HOME_Y);
	c->idle_count =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_IDLE_COUNT);
	c->charging =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_SENSE_ATTRIBUTE_CHARGING);

	/* Robot initial pose and battery */
	c->battery->item = (zephyrbt_node_context_t)(intptr_t)BATTERY_START;
	c->pos_x->item = (zephyrbt_node_context_t)(intptr_t)START_X;
	c->pos_y->item = (zephyrbt_node_context_t)(intptr_t)START_Y;
	c->tick_count->item = (zephyrbt_node_context_t)0;
	c->obstacle_dist->item =
		(zephyrbt_node_context_t)(intptr_t)compute_obstacle_dist(s, START_X, START_Y);

	/* Tree-level constants exposed to Lua predicates */
	c->delay_msec->item = (zephyrbt_node_context_t)(intptr_t)BT_DELAY_MSEC;
	c->charger_x->item = (zephyrbt_node_context_t)(intptr_t)CHARGER_X;
	c->charger_y->item = (zephyrbt_node_context_t)(intptr_t)CHARGER_Y;
	c->critical_battery->item = (zephyrbt_node_context_t)(intptr_t)CRITICAL_BATTERY;
	c->min_clearance->item = (zephyrbt_node_context_t)(intptr_t)MIN_CLEARANCE;
	c->safe_x->item = (zephyrbt_node_context_t)(intptr_t)SAFE_X;
	c->safe_y->item = (zephyrbt_node_context_t)(intptr_t)SAFE_Y;
	c->home_x->item = (zephyrbt_node_context_t)(intptr_t)HOME_X;
	c->home_y->item = (zephyrbt_node_context_t)(intptr_t)HOME_Y;
	c->idle_count->item = (zephyrbt_node_context_t)0;

	/* Cache bb pointers consumed by emergency + display */
	s->battery_item = c->battery;
	s->pos_x_item = c->pos_x;
	s->pos_y_item = c->pos_y;
	s->charging_item = c->charging;

	display_init();

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_sense(struct zephyrbt_context *ctx,
						 struct zephyrbt_node *self)
{
	struct sense_ctx *c = (struct sense_ctx *)self->ctx;
	struct robot_state *s = robot_get_state();

	int tick = display_get_tick() + 1;
	int px = (intptr_t)c->pos_x->item;
	int py = (intptr_t)c->pos_y->item;

	c->tick_count->item = (zephyrbt_node_context_t)(intptr_t)tick;
	c->obstacle_dist->item =
		(zephyrbt_node_context_t)(intptr_t)compute_obstacle_dist(s, px, py);
	c->charging->item = (zephyrbt_node_context_t)(intptr_t)s->charging;

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

/* ---- Respawn node ----------------------------------------------
 * When Lua signals bb.new_mission=1 (set in on_idle_tick), clear
 * the flag and regenerate obstacles/waypoints/targets for the next
 * mission.  Keeps the world-mutation decision visible in the tree.
 * -------------------------------------------------------------- */

struct respawn_ctx {
	struct zephyrbt_blackboard_item *new_mission;
};

enum zephyrbt_child_status zephyrbt_action_respawn_init(struct zephyrbt_context *ctx,
							struct zephyrbt_node *self)
{
	struct respawn_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->new_mission = zephyrbt_search_blackboard(ctx, self->index,
						    ZEPHYRBT_RESPAWN_ATTRIBUTE_NEW_MISSION);

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_respawn(struct zephyrbt_context *ctx,
						   struct zephyrbt_node *self)
{
	struct respawn_ctx *c = (struct respawn_ctx *)self->ctx;

	if ((intptr_t)c->new_mission->item == 1) {
		c->new_mission->item = (zephyrbt_node_context_t)0;
		robot_new_mission(robot_get_state());
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

/* ---- Render node -----------------------------------------------
 * Reads the current pose and battery and refreshes the terminal
 * display.  All other nodes update display state incrementally via
 * display_update_*; Render is the once-per-tick commit point.
 * -------------------------------------------------------------- */

struct render_ctx {
	struct zephyrbt_blackboard_item *pos_x;
	struct zephyrbt_blackboard_item *pos_y;
	struct zephyrbt_blackboard_item *battery;
	struct zephyrbt_blackboard_item *tick_count;
};

enum zephyrbt_child_status zephyrbt_action_render_init(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self)
{
	struct render_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->pos_x = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_RENDER_ATTRIBUTE_POS_X);
	c->pos_y = zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_RENDER_ATTRIBUTE_POS_Y);
	c->battery =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_RENDER_ATTRIBUTE_BATTERY);
	c->tick_count =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_RENDER_ATTRIBUTE_TICK_COUNT);

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_render(struct zephyrbt_context *ctx,
						  struct zephyrbt_node *self)
{
	struct render_ctx *c = (struct render_ctx *)self->ctx;

	int px = (intptr_t)c->pos_x->item;
	int py = (intptr_t)c->pos_y->item;
	int bat = (intptr_t)c->battery->item;
	int tick = (intptr_t)c->tick_count->item;

	display_render(robot_get_state(), px, py, bat, tick);

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
