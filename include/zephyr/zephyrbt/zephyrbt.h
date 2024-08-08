/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: UNLICENSED
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYRBT_ZEPHYRBT_H_
#define ZEPHYR_INCLUDE_ZEPHYRBT_ZEPHYRBT_H_

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Behaviour Tree API
 * @defgroup behaviour_tree_api Behaviour Tree API
 * @ingroup behaviour_tree
 * @{
 */

enum zephyrbt_child_status {
	ZEPHYRBT_CHILD_SUCCESS_STATUS,
	ZEPHYRBT_CHILD_RUNNING_STATUS,
	ZEPHYRBT_CHILD_FAILURE_STATUS,
	ZEPHYRBT_CHILD_SKIP_STATUS,
};

enum zephyrbt_blackboard_item_type {
	ZEPHYRBT_BLACKBOARD_ITEM_INPUT_TYPE,
	ZEPHYRBT_BLACKBOARD_ITEM_OUTPUT_TYPE,
	ZEPHYRBT_BLACKBOARD_ITEM_INOUT_TYPE,
};

typedef int16_t zephyrbt_node_idx_t;
typedef void *zephyrbt_node_context_t;

struct zephyrbt_node;
struct zephyrbt_blackboard_item;
struct zephyrbt_context;

typedef enum zephyrbt_child_status (*zephyrbt_tick_function_t)(struct zephyrbt_context *ctx,
							       struct zephyrbt_node *self);

struct zephyrbt_node {
	zephyrbt_tick_function_t function;
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT
	zephyrbt_tick_function_t init;
#endif
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT
	zephyrbt_node_context_t ctx;
#endif
	zephyrbt_node_idx_t sibling;
	zephyrbt_node_idx_t child;
	zephyrbt_node_idx_t index;
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO
	char *name;
#endif
};

struct zephyrbt_blackboard_item {
	const int idx;
	const int ref;
	const int key;
	zephyrbt_node_context_t item;
	const enum zephyrbt_blackboard_item_type type;
};

struct zephyrbt_context {
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO
	const char *name;
	size_t deep;
#endif
	struct zephyrbt_node *node;
	const size_t nodes;
	struct zephyrbt_blackboard_item *blackboard;
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_USER_DATA
	void *user_data;
#endif
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_DYNAMIC
	struct k_thread thread;
	k_thread_stack_t *stack;
	const int stack_size;
	const int thread_prio;
	k_tid_t tid;
#endif
};

// clang-format off

/**
 * @brief Macro for creating a Behaviour Tree instance. This is a convinience
 * tool to provide an easy to use case.
 *
 * @param _name		Name of the instance.
 * @param _nodes	The Behaviour Tree nodes structure.
 * @param _stack_size	Thread stack size.
 * @param _thread_prio  Thread priority.
 * @param _user_data	User defined data. Default is NULL.
 * @param _blackboard	The Behaviour Tree blackboard structure.
 */
#define ZEPHYRBT_DEFINE(_name, _nodes, _stack_size, _thread_prio, _user_data, _blackboard)	   \
	STRUCT_SECTION_ITERABLE(zephyrbt_context, _name) = {					   \
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (				   \
			.name = #_name,								   \
			.deep = 0, )								   \
		)										   \
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_USER_DATA, (				   \
			.user_data = _user_data, )						   \
		)										   \
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_DYNAMIC, (				   \
			.stack_size  = _stack_size,						   \
			.thread_prio = _thread_prio, )						   \
		)										   \
		.node       = _nodes,								   \
		.nodes      = ARRAY_SIZE(_nodes),						   \
		.blackboard = _blackboard,							   \
	}

// clang-format on

struct zephyrbt_node *zephyrbt_get_root(struct zephyrbt_context *ctx);
struct zephyrbt_node *zephyrbt_get_node(struct zephyrbt_context *ctx, int index);
struct zephyrbt_blackboard_item *zephyrbt_search_blackboard(struct zephyrbt_context *ctx,
							    const int index, const int key);
enum zephyrbt_child_status zephyrbt_evaluate(struct zephyrbt_context *ctx,
					     struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_action_always_success(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_action_always_failure(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self);
enum zephyrbt_sleep_attributes {
	ZEPHYRBT_SLEEP_ATTRIBUTE_MSEC,
};
enum zephyrbt_child_status zephyrbt_action_sleep(struct zephyrbt_context *ctx,
						 struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_action_sleep_init(struct zephyrbt_context *ctx,
						      struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_control_fallback(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_control_sequence(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self);

enum zephyrbt_child_status zephyrbt_decorator_inverter(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self);

#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT
enum zephyrbt_parallel_attributes {
	ZEPHYRBT_PARALLEL_ATTRIBUTE_FAILURE_COUNT,
	ZEPHYRBT_PARALLEL_ATTRIBUTE_SUCCESS_COUNT,
};

enum zephyrbt_child_status zephyrbt_control_parallel(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_control_parallel_init(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self);

enum zephyrbt_repeat_attributes {
	ZEPHYRBT_REPEAT_ATTRIBUTE_NUM_CYCLES,
};

enum zephyrbt_child_status zephyrbt_decorator_repeat(struct zephyrbt_context *ctx,
						     struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_repeat_init(struct zephyrbt_context *ctx,
							  struct zephyrbt_node *self);

enum zephyrbt_delay_attributes {
	ZEPHYRBT_DELAY_ATTRIBUTE_DELAY_MSEC,
};

enum zephyrbt_child_status zephyrbt_decorator_delay(struct zephyrbt_context *ctx,
						    struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_delay_init(struct zephyrbt_context *ctx,
							 struct zephyrbt_node *self);

enum zephyrbt_run_once_attributes {
	ZEPHYRBT_RUN_ONCE_ATTRIBUTE_THEN_SKIP,
};

enum zephyrbt_child_status zephyrbt_decorator_run_once(struct zephyrbt_context *ctx,
						       struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_run_once_init(struct zephyrbt_context *ctx,
							    struct zephyrbt_node *self);

enum zephyrbt_timeout_attributes {
	ZEPHYRBT_TIMEOUT_ATTRIBUTE_MSEC,
};

enum zephyrbt_child_status zephyrbt_decorator_timeout(struct zephyrbt_context *ctx,
						      struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_timeout_init(struct zephyrbt_context *ctx,
							   struct zephyrbt_node *self);
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_ZEPHYRBT_ZEPHYRBT_H_ */
