/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * The gate is open for the first OPEN_TICKS ticks, and Work runs. Then the
 * gate closes, and the tree halts the Timeout and Work. Report runs while the
 * gate is closed and, after CLOSED_TICKS ticks, logs:
 *
 *  - work: the number of calls to the halt function of Work. It must be 1:
 *    the tree halts Work once, and not again on the next ticks.
 *  - order: the value of work when _onHalted of the Timeout ran. It must be
 *    1: _onHalted of a node runs after its subtree halted.
 */

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zephyrbt/zephyrbt.h>
LOG_MODULE_REGISTER(halt, LOG_LEVEL_INF);

#include "halt.h"

#define OPEN_TICKS   3
#define CLOSED_TICKS 3

static int ticks;
static int closed_ticks;

enum zephyrbt_child_status zephyrbt_condition_gate_open(struct zephyrbt_context *ctx,
							const struct zephyrbt_node *self)
{
	++ticks;

	return ticks <= OPEN_TICKS ? ZEPHYRBT_CHILD_SUCCESS_STATUS : ZEPHYRBT_CHILD_FAILURE_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_work(struct zephyrbt_context *ctx,
						const struct zephyrbt_node *self)
{
	return ZEPHYRBT_CHILD_RUNNING_STATUS;
}

void zephyrbt_action_work_halt(struct zephyrbt_context *ctx, const struct zephyrbt_node *self)
{
	struct zephyrbt_blackboard_item *work_halts =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_WORK_ATTRIBUTE_WORK_HALTS);

	work_halts->item = (zephyrbt_node_context_t)((intptr_t)work_halts->item + 1);
}

enum zephyrbt_child_status zephyrbt_action_report(struct zephyrbt_context *ctx,
						  const struct zephyrbt_node *self)
{
	if (++closed_ticks != CLOSED_TICKS) {
		return ZEPHYRBT_CHILD_RUNNING_STATUS;
	}

	struct zephyrbt_blackboard_item *work_halts =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_REPORT_ATTRIBUTE_WORK_HALTS);
	struct zephyrbt_blackboard_item *order =
		zephyrbt_search_blackboard(ctx, self->index, ZEPHYRBT_REPORT_ATTRIBUTE_ORDER);

	LOG_INF("halt result: work=%d order=%d", (int)(intptr_t)work_halts->item,
		(int)(intptr_t)order->item);

	return ZEPHYRBT_CHILD_RUNNING_STATUS;
}

int main(void)
{
	return 0;
}
