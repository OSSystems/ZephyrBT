/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * The tree leaves a running Timeout and comes back to it later. The Timeout
 * must then start a new deadline, as BehaviorTree.CPP does when it halts the
 * node, and not use the deadline of the earlier run.
 *
 * Timeline, from the first tick:
 *   0 ms   - GATE_CLOSE_ms: the gate is open, Work runs inside the Timeout.
 *   GATE_CLOSE_ms - GATE_REOPEN_ms: the gate is closed, the tree runs Idle.
 *            The deadline of the first run (TIMEOUT_ms) expires here.
 *   GATE_REOPEN_ms: the gate opens again and Work runs inside the Timeout.
 *
 * ReportTimeout runs when the Timeout fails and tells how long the second run
 * of Work took.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zephyrbt/zephyrbt.h>
LOG_MODULE_REGISTER(timeout_halt, LOG_LEVEL_INF);

#define TICK_ms        5
#define TIMEOUT_ms     100
#define GATE_CLOSE_ms  50
#define GATE_REOPEN_ms 200

static int64_t start_ms = -1;
static int64_t reopen_ms = -1;
static bool done;

enum zephyrbt_child_status zephyrbt_condition_gate_open(struct zephyrbt_context *ctx,
							struct zephyrbt_node *self)
{
	int64_t now = k_uptime_get();

	if (start_ms < 0) {
		start_ms = now;
	}

	if (done) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	if (now - start_ms < GATE_CLOSE_ms) {
		return ZEPHYRBT_CHILD_SUCCESS_STATUS;
	}

	if (now - start_ms < GATE_REOPEN_ms) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	if (reopen_ms < 0) {
		reopen_ms = now;
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_work(struct zephyrbt_context *ctx,
						struct zephyrbt_node *self)
{
	k_msleep(TICK_ms);

	return ZEPHYRBT_CHILD_RUNNING_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_report_timeout(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
	if (reopen_ms < 0) {
		LOG_ERR("Timeout before the gate closed");
		done = true;
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	int64_t elapsed_ms = k_uptime_get() - reopen_ms;

	if (elapsed_ms >= TIMEOUT_ms) {
		LOG_INF("Timeout restarted: %lld ms", elapsed_ms);
	} else {
		LOG_ERR("Timeout kept the stale deadline: %lld ms", elapsed_ms);
	}

	done = true;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_idle(struct zephyrbt_context *ctx,
						struct zephyrbt_node *self)
{
	k_msleep(TICK_ms);

	return ZEPHYRBT_CHILD_RUNNING_STATUS;
}

int main(void)
{
	return 0;
}
