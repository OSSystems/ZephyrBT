/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt_controls, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

/*
 *       A [sequence]
 *      /
 *     B -   C  -  D -  E [siblings]
 *    /           /
 *   F - G       H
 *  /           /
 * I - J - K   L - M
 *
 * The root A is a sequence and [B, C, D, E] are their childs:
 *  - status == false
 *  - sibling = A->child
 *  - while(sibling)
 *  - 	status = sibling->function()
 *  -   if status == false
 *  -      return status
 *  -   sibling = sibling->sibling
 *  - return status
 */
enum zephyrbt_child_status zephyrbt_control_sequence(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	struct zephyrbt_node *siblings = zephyrbt_get_node(ctx, self->child);
	enum zephyrbt_child_status status = ZEPHYRBT_CHILD_FAILURE_STATUS;

	while (siblings) {
		status = zephyrbt_evaluate(ctx, siblings);

		switch (status) {
		case ZEPHYRBT_CHILD_RUNNING_STATUS:
			__fallthrough;
		case ZEPHYRBT_CHILD_FAILURE_STATUS:
			return status;
		case ZEPHYRBT_CHILD_SKIP_STATUS:
			__fallthrough;
		case ZEPHYRBT_CHILD_SUCCESS_STATUS:
			siblings = zephyrbt_get_node(ctx, siblings->sibling);
			break;
		}
	}

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

/*
 *       A [fallback]
 *      /
 *     B -   C  -  D -  E [siblings]
 *    /           /
 *   F - G       H
 *  /           /
 * I - J - K   L - M
 *
 * The root A is a fallback and [B, C, D, E] are their childs:
 *  - status == false
 *  - sibling = A->child
 *  - while(sibling)
 *  - 	status = sibling->function()
 *  -   if status == true
 *  -      return status
 *  -   sibling = sibling->sibling
 *  - return status
 */
enum zephyrbt_child_status zephyrbt_control_fallback(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	struct zephyrbt_node *siblings = zephyrbt_get_node(ctx, self->child);
	enum zephyrbt_child_status status = ZEPHYRBT_CHILD_FAILURE_STATUS;

	while (siblings) {
		status = zephyrbt_evaluate(ctx, siblings);

		switch (status) {
		case ZEPHYRBT_CHILD_RUNNING_STATUS:
			__fallthrough;
		case ZEPHYRBT_CHILD_SUCCESS_STATUS:
			return status;
		case ZEPHYRBT_CHILD_SKIP_STATUS:
			__fallthrough;
		case ZEPHYRBT_CHILD_FAILURE_STATUS:
			siblings = zephyrbt_get_node(ctx, siblings->sibling);
			break;
		}
	}

	return ZEPHYRBT_CHILD_FAILURE_STATUS;
}

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT)
enum zephyrbt_child_status zephyrbt_control_parallel_init(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("init: %s", self->name);
#endif

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}

enum zephyrbt_child_status zephyrbt_control_parallel(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	return ZEPHYRBT_CHILD_SUCCESS_STATUS;
}
#endif /* CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT */
