/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/logging/log.h>
#include "robot_state.h"

LOG_MODULE_REGISTER(robot_patrol, LOG_LEVEL_INF);

/* ANSI escape codes */
#define CSI        "\x1b["
#define CUR_HOME   CSI "H"
#define CLR_SCREEN CSI "2J"
#define CLR_LINE   CSI "K"
#define BOLD       CSI "1m"
#define DIM        CSI "2m"
#define RST        CSI "0m"
#define RED        CSI "31m"
#define GRN        CSI "32m"
#define YEL        CSI "33m"
#define CYN        CSI "36m"
#define MAG        CSI "35m"

/* Display state — updated by node tick functions */
static struct display_state {
	int target_wp;
	int target_x;
	int target_y;
	int step;
	int step_total;
	int scans_done;
	int targets_found;
	uint8_t visited;
	int last_scan_found;
	int scanning; /* 0=off, 1=arrived, 2=searching, 3=found */
	int last_tick;
	char status[48];
} g_disp;

void display_init(void)
{
	g_disp.target_wp = -1;
	g_disp.status[0] = '\0';
#ifndef CONFIG_ROBOT_PATROL_TEST_MODE
	printf(CLR_SCREEN);
	fflush(stdout);
#endif
}

void display_update_nav(int wp_idx, int tx, int ty, int step, int total)
{
	if (wp_idx >= 0) {
		g_disp.target_wp = wp_idx;
		g_disp.target_x = tx;
		g_disp.target_y = ty;
	}
	g_disp.step = step;
	g_disp.step_total = total;
}

void display_update_scan(int targets_total, int found)
{
	g_disp.targets_found = targets_total;
	g_disp.last_scan_found = found;
}

void display_tick(void)
{
	g_disp.last_tick++;
}

int display_get_tick(void)
{
	return g_disp.last_tick;
}

void display_set_scanning(int phase)
{
	g_disp.scanning = phase;
}

void display_update_status(const char *msg)
{
	int i;

	for (i = 0; i < 47 && msg[i]; i++) {
		g_disp.status[i] = msg[i];
	}
	g_disp.status[i] = '\0';
}

void display_set_visited(int wp_idx)
{
	if (wp_idx == -2) {
		/* Reset all */
		g_disp.visited = 0;
		g_disp.scans_done = 0;
		g_disp.targets_found = 0;
		g_disp.target_wp = -1;
		return;
	}
	if (wp_idx < 0) {
		wp_idx = g_disp.target_wp;
	}
	if (wp_idx >= 0 && wp_idx < NUM_WAYPOINTS) {
		if (!(g_disp.visited & (1 << wp_idx))) {
			g_disp.visited |= (1 << wp_idx);
			g_disp.scans_done++;
		}
	}
}

static int is_waypoint(struct robot_state *s, int x, int y, int *wp_idx)
{
	for (int i = 0; i < NUM_WAYPOINTS; i++) {
		if (s->waypoints[i][0] == x && s->waypoints[i][1] == y) {
			*wp_idx = i;
			return 1;
		}
	}
	return 0;
}

static void print_cell(struct robot_state *s, int x, int y, int rx, int ry)
{
	int wp;

	if (x == rx && y == ry) {
		if (g_disp.scanning == 1) {
			printf(BOLD MAG "? " RST);
		} else if (g_disp.scanning == 2) {
			printf(BOLD GRN "! " RST);
		} else {
			printf(BOLD GRN "R " RST);
		}
	} else if (x == CHARGER_X && y == CHARGER_Y) {
		printf(YEL "C " RST);
	} else if (x == HOME_X && y == HOME_Y) {
		printf(CYN "H " RST);
	} else if (is_waypoint(s, x, y, &wp)) {
		if (g_disp.visited & (1 << wp)) {
			printf(CYN "v " RST);
		} else {
			printf(YEL "W " RST);
		}
	} else if (s->grid[y][x] == CELL_OBSTACLE) {
		printf(RED "# " RST);
	} else if (s->grid[y][x] == CELL_TARGET) {
		printf(MAG "* " RST);
	} else {
		printf(DIM ". " RST);
	}
}

static void print_battery(char *buf, int sz, int battery)
{
	int fill = battery / 10;
	int pos = 0;

	pos += snprintf(buf + pos, sz - pos, "Bat [");
	for (int i = 0; i < 10 && pos < sz - 1; i++) {
		buf[pos++] = i < fill ? '=' : ' ';
	}
	pos += snprintf(buf + pos, sz - pos, "]%4d", battery);
	buf[pos] = '\0';
}

static const char *bat_color(int battery)
{
	if (battery > 50) {
		return GRN;
	} else if (battery > 20) {
		return YEL;
	}
	return RED;
}

/*
 * Right panel: fixed 26 visible chars so the border
 * aligns. ANSI codes don't count as visible chars.
 * Format: "  %-26s" but we build manually since
 * ANSI codes mess up printf width counting.
 */
static char rbuf[128];

static void print_right(int row, int battery, int pos_x, int pos_y, int tick)
{
	int n = 0;

	rbuf[0] = '\0';
	switch (row) {
	case 0:
		/* Battery rendered separately */
		break;
	case 1:
		n = snprintf(rbuf, sizeof(rbuf), "Pos     (%d,%d)", pos_x, pos_y);
		break;
	case 2:
		if (g_disp.target_wp >= 0) {
			n = snprintf(rbuf, sizeof(rbuf), "Target  W%d (%d,%d)",
				     g_disp.target_wp + 1, g_disp.target_x, g_disp.target_y);
		} else {
			n = snprintf(rbuf, sizeof(rbuf), "Target  --");
		}
		break;
	case 3:
		if (g_disp.step_total > 0) {
			n = snprintf(rbuf, sizeof(rbuf), "Step    %d/%d", g_disp.step,
				     g_disp.step_total);
		} else {
			n = snprintf(rbuf, sizeof(rbuf), "Step    --");
		}
		break;
	case 4:
		n = snprintf(rbuf, sizeof(rbuf), "Scanned %d/%d", g_disp.scans_done, NUM_WAYPOINTS);
		break;
	case 5:
		n = snprintf(rbuf, sizeof(rbuf), "Targets %d", g_disp.targets_found);
		break;
	case 6:
		n = snprintf(rbuf, sizeof(rbuf), "Tick    %d", tick);
		break;
	case 7:
		n = snprintf(rbuf, sizeof(rbuf), "%s", g_disp.status);
		break;
	default:
		break;
	}

	if (row == 0) {
		print_battery(rbuf, sizeof(rbuf), battery);
	}
	if (row == 0) {
		printf(" %s%s" RST, bat_color(battery), rbuf);
	} else {
		printf(" %s", rbuf);
	}
	printf(CLR_LINE CSI "47G|");
}

void display_render(struct robot_state *state, int pos_x, int pos_y, int battery, int tick)
{
	/* Tick always increases — take the max */
	if (tick > g_disp.last_tick) {
		g_disp.last_tick = tick;
	} else {
		tick = g_disp.last_tick;
	}

	/* Override status for celebration */
	if (g_disp.scans_done >= NUM_WAYPOINTS && pos_x == HOME_X && pos_y == HOME_Y &&
	    battery >= CRITICAL_BATTERY) {
		snprintf(g_disp.status, sizeof(g_disp.status), "Mission %d complete!",
			 state->mission_num);
	}

#ifdef CONFIG_ROBOT_PATROL_TEST_MODE
	LOG_INF("tick=%d pos=(%d,%d) bat=%d scan=%d/%d "
		"targets=%d status=%s",
		tick, pos_x, pos_y, battery, g_disp.scans_done, NUM_WAYPOINTS, g_disp.targets_found,
		g_disp.status[0] ? g_disp.status : "idle");
	return;
#endif
	if (g_disp.scans_done >= NUM_WAYPOINTS && pos_x == HOME_X && pos_y == HOME_Y &&
	    battery >= CRITICAL_BATTERY) {
		snprintf(g_disp.status, sizeof(g_disp.status), "Mission %d complete!",
			 state->mission_num);
	}

	printf(CUR_HOME);

	/* Title */
	printf("+---------------------------------------------+\n");
	printf("| " BOLD "ZephyrBT Robot Patrol" RST CSI "47G|\n");
	printf("+---------------------------------------------+\n");

	/* Column headers + battery */
	printf("|  " DIM " 0 1 2 3 4 5 6 7" RST);
	print_right(0, battery, pos_x, pos_y, tick);
	printf("\n");

	/* Grid rows + right panel */
	for (int y = 0; y < GRID_SIZE; y++) {
		printf("| " DIM "%d" RST " ", y);
		for (int x = 0; x < GRID_SIZE; x++) {
			print_cell(state, x, y, pos_x, pos_y);
		}
		print_right(y + 1, battery, pos_x, pos_y, tick);
		printf("\n");
	}

	/*
	 * Legend rows. Use CSI K (clear to EOL) then
	 * move cursor to column 47 for the right |.
	 */
	printf("| " BOLD GRN "R" RST "=Robot " YEL "C" RST "=Charger " CYN "H" RST "=Home " YEL
	       "W" RST "=Waypoint" CSI "47G|\n");
	printf("| " CYN "v" RST "=Visited " RED "#" RST "=Obstacle " MAG "*" RST "=Target" CSI
	       "47G|\n");

	printf("+---------------------------------------------+\n");

	fflush(stdout);
}
