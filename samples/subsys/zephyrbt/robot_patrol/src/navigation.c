/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(navigation, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "robot_patrol.h"
#include "robot_state.h"

/*
 * Per-instance path storage.
 * ComputePath and FollowPath within the same NavigateTo
 * subtree share a nav_status bb item (port aliased).
 * We use that pointer as a key to look up the shared path.
 */
#define MAX_NAV_INSTANCES 4

static struct robot_path g_nav_paths[MAX_NAV_INSTANCES];
static struct zephyrbt_blackboard_item *g_nav_keys[MAX_NAV_INSTANCES];
static int g_nav_count;

static struct robot_path *get_nav_path(struct zephyrbt_blackboard_item *nav_item)
{
	for (int i = 0; i < g_nav_count; i++) {
		if (g_nav_keys[i] == nav_item) {
			return &g_nav_paths[i];
		}
	}

	if (g_nav_count >= MAX_NAV_INSTANCES) {
		return &g_nav_paths[0];
	}

	g_nav_keys[g_nav_count] = nav_item;
	return &g_nav_paths[g_nav_count++];
}

/*
 * BFS pathfinding on the 8x8 grid.
 * Finds shortest path from (from_x,from_y) to (to_x,to_y).
 * Stores result in the provided path struct.
 * Returns path length or -1.
 */
int robot_bfs(struct robot_state *state, int from_x, int from_y, int to_x, int to_y)
{
	struct robot_path *p = &state->path;
	int8_t visited[GRID_SIZE][GRID_SIZE];
	int8_t parent_x[GRID_SIZE][GRID_SIZE];
	int8_t parent_y[GRID_SIZE][GRID_SIZE];
	int8_t queue_x[GRID_SIZE * GRID_SIZE];
	int8_t queue_y[GRID_SIZE * GRID_SIZE];
	int head = 0;
	int tail = 0;

	/* 4-connected neighbors: right, down, left, up */
	static const int8_t dx[] = {1, 0, -1, 0};
	static const int8_t dy[] = {0, 1, 0, -1};

	memset(visited, 0, sizeof(visited));
	memset(parent_x, -1, sizeof(parent_x));
	memset(parent_y, -1, sizeof(parent_y));

	if (from_x == to_x && from_y == to_y) {
		p->len = 0;
		p->idx = 0;
		p->valid = true;
		return 0;
	}

	queue_x[tail] = from_x;
	queue_y[tail] = from_y;
	tail++;
	visited[from_y][from_x] = 1;

	while (head < tail) {
		int cx = queue_x[head];
		int cy = queue_y[head];

		head++;

		for (int d = 0; d < 4; d++) {
			int nx = cx + dx[d];
			int ny = cy + dy[d];

			if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE) {
				continue;
			}

			if (visited[ny][nx]) {
				continue;
			}

			if (state->grid[ny][nx] == CELL_OBSTACLE) {
				continue;
			}

			visited[ny][nx] = 1;
			parent_x[ny][nx] = cx;
			parent_y[ny][nx] = cy;

			if (nx == to_x && ny == to_y) {
				goto found;
			}

			queue_x[tail] = nx;
			queue_y[tail] = ny;
			tail++;
		}
	}

	p->valid = false;
	return -1;

found:
	p->len = 0;
	{
		int cx = to_x;
		int cy = to_y;

		while (cx != from_x || cy != from_y) {
			if (p->len >= MAX_PATH) {
				p->valid = false;
				return -1;
			}
			p->steps[p->len][0] = cx;
			p->steps[p->len][1] = cy;
			p->len++;

			int px = parent_x[cy][cx];
			int py = parent_y[cy][cx];

			cx = px;
			cy = py;
		}
	}

	/* Reverse the path (it was built backward) */
	for (int i = 0; i < p->len / 2; i++) {
		int j = p->len - 1 - i;
		int8_t tx = p->steps[i][0];
		int8_t ty = p->steps[i][1];

		p->steps[i][0] = p->steps[j][0];
		p->steps[i][1] = p->steps[j][1];
		p->steps[j][0] = tx;
		p->steps[j][1] = ty;
	}

	p->idx = 0;
	p->valid = true;
	return p->len;
}

/* ---- ComputePath node ---- */

struct compute_path_ctx {
	struct zephyrbt_blackboard_item *target_x;
	struct zephyrbt_blackboard_item *target_y;
	struct zephyrbt_blackboard_item *pos_x;
	struct zephyrbt_blackboard_item *pos_y;
	struct zephyrbt_blackboard_item *nav_status;
};

enum zephyrbt_child_status zephyrbt_action_compute_path_init(struct zephyrbt_context *ctx,
							     struct zephyrbt_node *self)
{
	struct compute_path_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->target_x = zephyrbt_search_blackboard(ctx, self->index,
						 ZEPHYRBT_COMPUTE_PATH_ATTRIBUTE_TARGET_X);
	c->target_y = zephyrbt_search_blackboard(ctx, self->index,
						 ZEPHYRBT_COMPUTE_PATH_ATTRIBUTE_TARGET_Y);
	c->pos_x =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_COMPUTE_PATH_ATTRIBUTE_POS_X);
	c->pos_y =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_COMPUTE_PATH_ATTRIBUTE_POS_Y);
	c->nav_status = zephyrbt_search_blackboard(ctx, self->index,
						   ZEPHYRBT_COMPUTE_PATH_ATTRIBUTE_NAV_STATUS);

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_compute_path(struct zephyrbt_context *ctx,
							struct zephyrbt_node *self)
{
	struct compute_path_ctx *c = (struct compute_path_ctx *)self->ctx;
	struct robot_state *s = robot_get_state();

	int fx = (intptr_t)c->pos_x->item;
	int fy = (intptr_t)c->pos_y->item;
	int tx = (intptr_t)c->target_x->item;
	int ty = (intptr_t)c->target_y->item;
	int len = robot_bfs(s, fx, fy, tx, ty);

	if (len < 0) {
		LOG_ERR("compute_path: no path "
			"(%d,%d)->(%d,%d)",
			fx, fy, tx, ty);
		c->nav_status->item = (zephyrbt_node_context_t)(intptr_t)NAV_IDLE;
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	/* Copy BFS result to per-instance path */
	struct robot_path *np = get_nav_path(c->nav_status);
	memcpy(np, &s->path, sizeof(*np));

	c->nav_status->item = (zephyrbt_node_context_t)(intptr_t)NAV_FOLLOWING;

	/* Clear any scan animation left over from a scan
	 * aborted by _failureIf (e.g. battery drained below
	 * threshold mid-scan) so the robot renders as R
	 * while moving, not ? or !. */
	display_set_scanning(0);
	display_update_status("Computing path...");

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

/* ---- FollowPath node ---- */

struct follow_path_ctx {
	struct zephyrbt_blackboard_item *pos_x;
	struct zephyrbt_blackboard_item *pos_y;
	struct zephyrbt_blackboard_item *battery;
	struct zephyrbt_blackboard_item *nav_status;
};

enum zephyrbt_child_status zephyrbt_action_follow_path_init(struct zephyrbt_context *ctx,
							    struct zephyrbt_node *self)
{
	struct follow_path_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->pos_x =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_FOLLOW_PATH_ATTRIBUTE_POS_X);
	c->pos_y =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_FOLLOW_PATH_ATTRIBUTE_POS_Y);
	c->battery = zephyrbt_search_blackboard(ctx, self->index,
						ZEPHYRBT_FOLLOW_PATH_ATTRIBUTE_BATTERY);
	c->nav_status = zephyrbt_search_blackboard(ctx, self->index,
						   ZEPHYRBT_FOLLOW_PATH_ATTRIBUTE_NAV_STATUS);

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_follow_path(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self)
{
	struct follow_path_ctx *c = (struct follow_path_ctx *)self->ctx;

	struct robot_path *p = get_nav_path(c->nav_status);

	if (!p->valid || p->idx >= p->len) {
		int px = (intptr_t)c->pos_x->item;
		int py = (intptr_t)c->pos_y->item;

		c->nav_status->item = (zephyrbt_node_context_t)(intptr_t)NAV_ARRIVED;
		display_update_nav(-1, 0, 0, 0, 0);
		if (px == HOME_X && py == HOME_Y) {
			display_update_status("Arrived home");
		}
		return ZEPHYRBT_CHILD_SUCCESS_STATUS;
	}

	int new_x = p->steps[p->idx][0];
	int new_y = p->steps[p->idx][1];

	p->idx++;

	c->pos_x->item = (zephyrbt_node_context_t)(intptr_t)new_x;
	c->pos_y->item = (zephyrbt_node_context_t)(intptr_t)new_y;

	/* Drain battery per step */
	intptr_t bat = (intptr_t)c->battery->item;

	bat -= BATTERY_MOVE;
	if (bat < 0) {
		bat = 0;
	}
	c->battery->item = (zephyrbt_node_context_t)bat;

	display_update_nav(-1, 0, 0, p->idx, p->len);
	display_update_status(bat < CRITICAL_BATTERY ? "Emergency: moving" : "Navigating...");

	if (p->idx >= p->len) {
		/* Last step moved — return RUNNING so
		 * UpdateWorld renders R here next tick.
		 * The exhausted-path check at the top
		 * will return SUCCESS on the next tick. */
		p->valid = false;
	}

	return ZEPHYRBT_CHILD_RUNNING_STATUS;
}
