/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt_action_set_blackboard, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

struct zephyrbt_action_set_blackboard_context {
	struct zephyrbt_blackboard_item *value;
	struct zephyrbt_blackboard_item *output_key;
};

enum zephyrbt_child_status zephyrbt_action_set_blackboard_init(struct zephyrbt_context *ctx,
							       struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("init: %s", self->name);
#endif

	struct zephyrbt_action_set_blackboard_context *set_blackboard;

	set_blackboard = k_malloc(sizeof(struct zephyrbt_action_set_blackboard_context));
	self->ctx = set_blackboard;

	if (set_blackboard == NULL) {
		LOG_ERR("Context can not be allocate.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	memset(set_blackboard, 0, sizeof(struct zephyrbt_action_set_blackboard_context));

	set_blackboard->value = zephyrbt_search_blackboard(ctx, self->index,
							   ZEPHYRBT_SET_BLACKBOARD_ATTRIBUTE_VALUE);

	if (set_blackboard->value == NULL) {
		LOG_ERR("Invalid set_blackboard value attibute.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	set_blackboard->output_key = zephyrbt_search_blackboard(
		ctx, self->index, ZEPHYRBT_SET_BLACKBOARD_ATTRIBUTE_OUTPUT_KEY);

	if (set_blackboard->output_key == NULL) {
		LOG_ERR("Invalid set_blackboard output_key attibute.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_action_set_blackboard(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	struct zephyrbt_action_set_blackboard_context *set_blackboard;
	set_blackboard = (struct zephyrbt_action_set_blackboard_context *)self->ctx;

	if (set_blackboard == NULL) {
		LOG_ERR("Undefined behaviour on zephyrbt_action_set_blackboard.");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	set_blackboard->output_key->item = set_blackboard->value->item;

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
