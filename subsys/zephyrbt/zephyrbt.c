/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyrbt, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

inline struct zephyrbt_node *zephyrbt_get_root(struct zephyrbt_context *ctx)
{
	return &ctx->node[ctx->nodes - 1];
}

inline struct zephyrbt_node *zephyrbt_get_node(struct zephyrbt_context *ctx, int index)
{
	/* ARRAY_SIZE is size_t and negative value MUST BE checked first */
	if (index < 0) {
		return NULL;
	}

	if (index >= ctx->nodes) {
		LOG_ERR("Index is out of range.");
		return NULL;
	}

	return &ctx->node[index];
}

inline struct zephyrbt_blackboard_item *zephyrbt_search_blackboard(struct zephyrbt_context *ctx,
								   const int index, const int key)
{
	for (struct zephyrbt_blackboard_item *entry = ctx->blackboard;
	     entry != NULL && entry->idx != -1; ++entry) {
		if (entry->idx == index && entry->key == key) {
			return entry;
		}
	}

	return NULL;
}

inline enum zephyrbt_child_status zephyrbt_evaluate(struct zephyrbt_context *ctx,
						    struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("%s", self->name);
#endif

	if (self == NULL) {
		LOG_ERR("Self can not be NULL");
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

	if (self->function == NULL) {
		LOG_ERR("Function on index %d is NULL", self->index);
		return ZEPHYRBT_CHILD_FAILURE_STATUS;
	}

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	++ctx->deep;
	LOG_DBG("Deep: %d", ctx->deep);
#endif

	enum zephyrbt_child_status status = self->function(ctx, self);

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	--ctx->deep;
#endif

	return status;
}

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_DYNAMIC)
void zephyrbt_thread_func(void *zephyrbt_ctx, void *, void *)
{
	struct zephyrbt_context *ctx = (struct zephyrbt_context *)zephyrbt_ctx;
	if (ctx == NULL) {
		LOG_ERR("The behavior tree context is invalid. Thread aborted!");
		return;
	}

	if (ctx->node == NULL) {
		LOG_ERR("The behavior tree API is invalid. Thread aborted!");
		return;
	}

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT)
	int i = 0;
	struct zephyrbt_node *root = zephyrbt_get_root(ctx);
	struct zephyrbt_node *self = zephyrbt_get_node(ctx, i);

	while (self != NULL && self != root) {
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT)
		self->ctx = NULL;
#endif

		if (self->init != NULL) {
			self->init(ctx, self);
		}

		self = zephyrbt_get_node(ctx, ++i);
	}
#endif

	while (true) {
		LOG_DBG("tick");
		zephyrbt_evaluate(ctx, zephyrbt_get_root(ctx));
		k_sleep(K_MSEC(1000));
	}
}

static int zephyrbt_init(void)
{
	STRUCT_SECTION_FOREACH(zephyrbt_context, ctx) {
		ctx->stack = k_thread_stack_alloc(ctx->stack_size, 0);
		if (ctx->stack == NULL) {
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
			LOG_ERR("No memory available to create thread stack for zephyrbt "
				"%s.",
				ctx->name);
#else
			LOG_ERR("No memory available to create zephyrbt thread stack.");
#endif
			return -ENOMEM;
		}

		ctx->tid = k_thread_create(&ctx->thread, ctx->stack, ctx->stack_size,
					   zephyrbt_thread_func, ctx, NULL, NULL, ctx->thread_prio,
					   K_INHERIT_PERMS, K_NO_WAIT);

		if (ctx->tid == NULL) {
			k_thread_stack_free(ctx->stack);

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
			LOG_ERR("Failed to create thread for zephyrbt %s.", ctx->name);
#else
			LOG_ERR("Failed to create zephyrbt thread.");
#endif

			return -ENOEXEC;
		}

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
		k_thread_name_set(&ctx->thread, ctx->name);
#endif
	}

	return 0;
}

SYS_INIT(zephyrbt_init, POST_KERNEL, CONFIG_ZEPHYR_BEHAVIOUR_TREE_INIT_PRIORITY);

#endif /* CONFIG_ZEPHYR_BEHAVIOUR_TREE_DYNAMIC */
