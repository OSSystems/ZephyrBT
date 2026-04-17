/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ROBOT_STATE_H_
#define ROBOT_STATE_H_

#include <stdint.h>
#include <stdbool.h>

struct zephyrbt_blackboard_item;

#define GRID_SIZE     8
#define MAX_PATH      64
#define NUM_WAYPOINTS 4
#define BATTERY_START 100
#define BATTERY_MOVE  1
#define BATTERY_SCAN  5
#define SCAN_RADIUS   1 /* 3x3 area */

/* Grid cell values */
#define CELL_FREE     0
#define CELL_OBSTACLE 1
#define CELL_TARGET   2

/* Navigation status */
#define NAV_IDLE      0
#define NAV_COMPUTING 1
#define NAV_FOLLOWING 2
#define NAV_ARRIVED   3

/* Fixed positions */
#define CHARGER_X 0
#define CHARGER_Y 0
#define HOME_X    0
#define HOME_Y    7
#define SAFE_X    4
#define SAFE_Y    4
#define START_X   0
#define START_Y   7

/* Thresholds */
#define CRITICAL_BATTERY 20
#define MIN_CLEARANCE    1

struct robot_path {
	int8_t steps[MAX_PATH][2];
	int8_t len;
	int8_t idx;
	bool valid;
};

struct robot_state {
	uint8_t grid[GRID_SIZE][GRID_SIZE];
	int8_t waypoints[NUM_WAYPOINTS][2];
	struct robot_path path;
	int16_t total_targets;
	struct zephyrbt_blackboard_item *battery_item;
	struct zephyrbt_blackboard_item *pos_x_item;
	struct zephyrbt_blackboard_item *pos_y_item;
	struct zephyrbt_blackboard_item *charging_item;
	int16_t mission_num;
	int8_t charging;
	bool initialized;
};

/* BFS pathfinding on the grid. Returns path length or -1. */
int robot_bfs(struct robot_state *state, int from_x, int from_y, int to_x, int to_y);

/* Get the global robot state. */
struct robot_state *robot_get_state(void);

/* Randomize targets for a new mission. */
void robot_new_mission(struct robot_state *state);

/* Terminal UI display functions */
void display_init(void);
void display_tick(void);
int display_get_tick(void);
void display_render(struct robot_state *state, int pos_x, int pos_y, int battery, int tick);
void display_update_nav(int wp_idx, int tx, int ty, int step, int total);
void display_update_scan(int targets_total, int found);
void display_set_scanning(int phase);
void display_update_status(const char *msg);
void display_set_visited(int wp_idx);

#endif /* ROBOT_STATE_H_ */
