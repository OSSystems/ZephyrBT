/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(emergency, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

#include "robot_patrol.h"
#include "robot_state.h"

/*
 * CheckMetric: compare metric against threshold.  Returns SUCCESS
 * (emergency!) when the metric drops below the threshold or while
 * charging is already in progress (so the HandleEmergency subtree
 * completes its charge cycle before yielding to patrol).
 *
 * On SUCCESS, announces "Emergency: moving" so the follow-up
 * NavigateTo instance paints the correct status while it steps.
 */

struct check_metric_ctx {
	struct zephyrbt_blackboard_item *metric;
	struct zephyrbt_blackboard_item *threshold;
	struct zephyrbt_blackboard_item *charging;
};

enum zephyrbt_child_status zephyrbt_action_check_metric_init(struct zephyrbt_context *ctx,
							     struct zephyrbt_node *self)
{
	struct check_metric_ctx *c = k_malloc(sizeof(*c));

	if (c == NULL) {
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	c->metric = zephyrbt_search_blackboard(ctx, self->index,
					       ZEPHYRBT_CHECK_METRIC_ATTRIBUTE_METRIC);
	c->threshold = zephyrbt_search_blackboard(ctx, self->index,
						  ZEPHYRBT_CHECK_METRIC_ATTRIBUTE_THRESHOLD);
	c->charging = zephyrbt_search_blackboard(ctx, self->index,
						 ZEPHYRBT_CHECK_METRIC_ATTRIBUTE_CHARGING);

	self->ctx = c;
	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_check_metric(struct zephyrbt_context *ctx,
							struct zephyrbt_node *self)
{
	struct check_metric_ctx *c = (struct check_metric_ctx *)self->ctx;

	int metric = (intptr_t)c->metric->item;
	int threshold = (intptr_t)c->threshold->item;
	int charging = (intptr_t)c->charging->item;

	if (metric < threshold || charging > 0) {
		display_update_status("Emergency: moving");
		return ZEPHYRBT_CHILD_SUCCESS_STATUS;
	}

	return ZEPHYRBT_CHILD_FAILURE_STATUS;
}
