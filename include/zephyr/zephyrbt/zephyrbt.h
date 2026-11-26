/*
 * Copyright (c) 2024-2026 O.S. Systems Software LTDA.
 * Copyright (c) 2024-2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_ZEPHYRBT_ZEPHYRBT_H_
#define ZEPHYR_INCLUDE_ZEPHYRBT_ZEPHYRBT_H_

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS
#include <lua.h>
#include <lauxlib.h>
#endif

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

enum zephyrbt_blackboard_item_entry {
	ZEPHYRBT_BLACKBOARD_ITEM_ENTRY_KEY,
};

typedef int16_t zephyrbt_node_idx_t;
typedef void *zephyrbt_node_context_t;

struct zephyrbt_node;
struct zephyrbt_blackboard_item;
struct zephyrbt_context;

typedef enum zephyrbt_child_status (*zephyrbt_tick_function_t)(struct zephyrbt_context *ctx,
							       const struct zephyrbt_node *self);
typedef void (*zephyrbt_halt_function_t)(struct zephyrbt_context *ctx,
					 const struct zephyrbt_node *self);

struct zephyrbt_node {
	zephyrbt_tick_function_t function;
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT
	zephyrbt_tick_function_t init;
#endif
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_HALT
	zephyrbt_halt_function_t halt;
#endif
	zephyrbt_node_idx_t sibling;
	zephyrbt_node_idx_t child;
	zephyrbt_node_idx_t index;
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO
	const char *name;
#endif
};

struct zephyrbt_blackboard_item {
	const int idx;
	const int ref;
	const int key;
	zephyrbt_node_context_t item;
	const enum zephyrbt_blackboard_item_type type;
};

#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS
enum zephyrbt_condition_type {
	ZEPHYRBT_COND_FAILURE_IF,
	ZEPHYRBT_COND_SUCCESS_IF,
	ZEPHYRBT_COND_SKIP_IF,
	ZEPHYRBT_COND_WHILE,
	ZEPHYRBT_COND_ON_HALTED,
	ZEPHYRBT_COND_ON_FAILURE,
	ZEPHYRBT_COND_ON_SUCCESS,
	ZEPHYRBT_COND_POST,
	ZEPHYRBT_COND_COUNT,
};

struct zephyrbt_node_conditions {
	int refs[ZEPHYRBT_COND_COUNT];
};

struct zephyrbt_lua_cond_ref {
	const int node_idx;
	const int cond_type;
	const char *func_name;
};

struct zephyrbt_bb_name_entry {
	const char *name;
	const int bb_idx;
};
#endif /* CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS */

struct zephyrbt_context {
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO
	const char *name;
	uint32_t deep;
#endif
	/* The nodes are constant, so that the table can be in flash. */
	const struct zephyrbt_node *node;
	const uint32_t nodes;
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT
	/* The context of each node, indexed by the node index. */
	zephyrbt_node_context_t *node_ctx;
#endif
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_HALT
	/* One bit for each node, set while the node is running. */
	atomic_t *running;
#endif
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
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_ALLOW_YIELD
	bool thread_yield;
#endif
#endif
#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS
	lua_State *lua;
	int bb_ref;
	struct zephyrbt_node_conditions *conditions;
	const struct zephyrbt_lua_cond_ref *lua_cond_refs;
	const int lua_cond_ref_count;
	const struct zephyrbt_bb_name_entry *bb_names;
	const int bb_names_count;
	const char *lua_gen_script;
	const char *const *lua_user_scripts;
	int lua_user_script_count;
#endif
};

// clang-format off

/**
 * @brief Macro for creating a Behaviour Tree instance. This is a convinience
 * tool to provide an easy to use case.
 *
 * The node contexts and the running flags are the arrays _name##_node_ctx
 * and _name##_running, which the generator defines with
 * ZEPHYRBT_NODE_DATA_DEFINE().
 *
 * @param _name		Name of the instance.
 * @param _nodes	The Behaviour Tree nodes structure.
 * @param _stack_size	Thread stack size.
 * @param _thread_prio  Thread priority.
 * @param _thread_yield Thread should yield. Default is True.
 * @param _user_data	User defined data. Default is NULL.
 * @param _blackboard	The Behaviour Tree blackboard structure.
 */
#define ZEPHYRBT_DEFINE(_name, _nodes, _stack_size, _thread_prio,				   \
			_thread_yield, _user_data, _blackboard)					   \
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
			.thread_prio = _thread_prio,						   \
			IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_ALLOW_YIELD, (			   \
				.thread_yield = _thread_yield, )				   \
			))									   \
		)										   \
		.node       = _nodes,								   \
		.nodes      = ARRAY_SIZE(_nodes),						   \
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT, (				   \
			.node_ctx = _name##_node_ctx, )						   \
		)										   \
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_HALT, (				   \
			.running = _name##_running, )						   \
		)										   \
		.blackboard = _blackboard,							   \
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS, (			   \
			.lua = NULL,								   \
			.bb_ref = LUA_NOREF,							   \
			.conditions = _name##_conditions,					   \
			.lua_cond_refs = _name##_lua_cond_refs,					   \
			.lua_cond_ref_count = ARRAY_SIZE(_name##_lua_cond_refs),		   \
			.bb_names = _name##_bb_names,						   \
			.bb_names_count = ARRAY_SIZE(_name##_bb_names),				   \
			.lua_gen_script = _name##_gen_lua_script,				   \
			.lua_user_scripts = NULL,						   \
			.lua_user_script_count = 0,						   \
		))										   \
	}

/**
 * @brief Define the RAM data of the nodes of a Behaviour Tree instance.
 *
 * @param _name		Name of the instance, as given to ZEPHYRBT_DEFINE().
 * @param _count	Number of nodes.
 */
#define ZEPHYRBT_NODE_DATA_DEFINE(_name, _count)						   \
	IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT, (					   \
		static zephyrbt_node_context_t _name##_node_ctx[_count];			   \
	))											   \
	IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_HALT, (					   \
		static ATOMIC_DEFINE(_name##_running, _count);					   \
	))

// clang-format on

const struct zephyrbt_node *zephyrbt_get_root(struct zephyrbt_context *ctx);
const struct zephyrbt_node *zephyrbt_get_node(struct zephyrbt_context *ctx, int index);
struct zephyrbt_blackboard_item *zephyrbt_search_blackboard(struct zephyrbt_context *ctx,
							    const int index, const int key);
enum zephyrbt_child_status zephyrbt_evaluate(struct zephyrbt_context *ctx,
					     const struct zephyrbt_node *self);

#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT
/**
 * @brief Get the context of a node.
 *
 * @param ctx	The behaviour tree.
 * @param self	The node.
 *
 * @return The context that zephyrbt_node_ctx_set() stored, or NULL.
 */
static inline zephyrbt_node_context_t zephyrbt_node_ctx_get(struct zephyrbt_context *ctx,
							    const struct zephyrbt_node *self)
{
	return ctx->node_ctx[self->index];
}

/**
 * @brief Set the context of a node.
 *
 * @param ctx	The behaviour tree.
 * @param self	The node.
 * @param value	The context, for example the memory that the init function
 *		allocates.
 */
static inline void zephyrbt_node_ctx_set(struct zephyrbt_context *ctx,
					 const struct zephyrbt_node *self,
					 zephyrbt_node_context_t value)
{
	ctx->node_ctx[self->index] = value;
}
#endif

#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_HALT
/**
 * @brief Halt a running node and its subtree.
 *
 * Mark the node as not running, call its halt function, and then halt its
 * running children in the same way. The Lua _onHalted condition of a node
 * runs after its subtree halted. Do nothing when the node is not running.
 *
 * @param ctx	The behaviour tree.
 * @param self	The node to halt.
 */
void zephyrbt_halt(struct zephyrbt_context *ctx, const struct zephyrbt_node *self);

/**
 * @brief Halt the running children of a node.
 *
 * @param ctx	The behaviour tree.
 * @param self	The node whose children are halted.
 */
void zephyrbt_halt_children(struct zephyrbt_context *ctx, const struct zephyrbt_node *self);

/**
 * @brief Halt the running siblings that follow a node.
 *
 * A control node calls this when it stops at a child, so that the children
 * after it do not keep the state of an earlier run.
 *
 * @param ctx	The behaviour tree.
 * @param self	The child at which the control node stopped.
 */
void zephyrbt_halt_next_siblings(struct zephyrbt_context *ctx, const struct zephyrbt_node *self);
#else
static inline void zephyrbt_halt(struct zephyrbt_context *ctx, const struct zephyrbt_node *self)
{
}

static inline void zephyrbt_halt_children(struct zephyrbt_context *ctx,
					  const struct zephyrbt_node *self)
{
}

static inline void zephyrbt_halt_next_siblings(struct zephyrbt_context *ctx,
					       const struct zephyrbt_node *self)
{
}
#endif
enum zephyrbt_child_status zephyrbt_action_always_success(struct zephyrbt_context *ctx,
							  const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_action_always_failure(struct zephyrbt_context *ctx,
							  const struct zephyrbt_node *self);
enum zephyrbt_set_blackboard_attributes {
	ZEPHYRBT_SET_BLACKBOARD_ATTRIBUTE_VALUE,
	ZEPHYRBT_SET_BLACKBOARD_ATTRIBUTE_OUTPUT_KEY,
};
enum zephyrbt_child_status zephyrbt_action_set_blackboard(struct zephyrbt_context *ctx,
							  const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_action_set_blackboard_init(struct zephyrbt_context *ctx,
							       const struct zephyrbt_node *self);

enum zephyrbt_sleep_attributes {
	ZEPHYRBT_SLEEP_ATTRIBUTE_MSEC,
};
enum zephyrbt_child_status zephyrbt_action_sleep(struct zephyrbt_context *ctx,
						 const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_action_sleep_init(struct zephyrbt_context *ctx,
						      const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_control_fallback(struct zephyrbt_context *ctx,
						     const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_control_sequence(struct zephyrbt_context *ctx,
						     const struct zephyrbt_node *self);

enum zephyrbt_child_status zephyrbt_decorator_inverter(struct zephyrbt_context *ctx,
						       const struct zephyrbt_node *self);

#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT
enum zephyrbt_parallel_attributes {
	ZEPHYRBT_PARALLEL_ATTRIBUTE_FAILURE_COUNT,
	ZEPHYRBT_PARALLEL_ATTRIBUTE_SUCCESS_COUNT,
};

enum zephyrbt_child_status zephyrbt_control_parallel(struct zephyrbt_context *ctx,
						     const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_control_parallel_init(struct zephyrbt_context *ctx,
							  const struct zephyrbt_node *self);

enum zephyrbt_repeat_attributes {
	ZEPHYRBT_REPEAT_ATTRIBUTE_NUM_CYCLES,
};

enum zephyrbt_child_status zephyrbt_decorator_repeat(struct zephyrbt_context *ctx,
						     const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_repeat_init(struct zephyrbt_context *ctx,
							  const struct zephyrbt_node *self);

enum zephyrbt_delay_attributes {
	ZEPHYRBT_DELAY_ATTRIBUTE_DELAY_MSEC,
};

enum zephyrbt_child_status zephyrbt_decorator_delay(struct zephyrbt_context *ctx,
						    const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_delay_init(struct zephyrbt_context *ctx,
							 const struct zephyrbt_node *self);

enum zephyrbt_run_once_attributes {
	ZEPHYRBT_RUN_ONCE_ATTRIBUTE_THEN_SKIP,
};

enum zephyrbt_child_status zephyrbt_decorator_run_once(struct zephyrbt_context *ctx,
						       const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_run_once_init(struct zephyrbt_context *ctx,
							    const struct zephyrbt_node *self);

enum zephyrbt_timeout_attributes {
	ZEPHYRBT_TIMEOUT_ATTRIBUTE_MSEC,
};

enum zephyrbt_child_status zephyrbt_decorator_timeout(struct zephyrbt_context *ctx,
						      const struct zephyrbt_node *self);
void zephyrbt_decorator_timeout_halt(struct zephyrbt_context *ctx,
				     const struct zephyrbt_node *self);
enum zephyrbt_child_status zephyrbt_decorator_timeout_init(struct zephyrbt_context *ctx,
							   const struct zephyrbt_node *self);
#endif

#ifdef CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS
int zephyrbt_lua_init(struct zephyrbt_context *ctx);
enum zephyrbt_child_status zephyrbt_lua_pre_check(struct zephyrbt_context *ctx,
						  const struct zephyrbt_node *self);
void zephyrbt_lua_post_check(struct zephyrbt_context *ctx, const struct zephyrbt_node *self,
			     enum zephyrbt_child_status status);
bool zephyrbt_lua_while_check(struct zephyrbt_context *ctx, const struct zephyrbt_node *self);
void zephyrbt_lua_on_halted(struct zephyrbt_context *ctx, const struct zephyrbt_node *self);
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_ZEPHYRBT_ZEPHYRBT_H_ */
