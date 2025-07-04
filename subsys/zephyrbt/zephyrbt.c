/*
 * Copyright (c) 2024-2025 O.S. Systems Software LTDA.
 * Copyright (c) 2024-2025 Freedom Veiculos Eletricos
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
	int reference = -1;

	for (struct zephyrbt_blackboard_item *entry = ctx->blackboard;
	     entry != NULL && entry->idx != -1; ++entry) {
		if (entry->idx == index && entry->key == key) {
			if (entry->ref == -1) {
				return entry;
			}

			reference = entry->ref;
			break;
		}
	}

	for (struct zephyrbt_blackboard_item *entry = ctx->blackboard;
	     entry != NULL && entry->idx != -1; ++entry) {
		if (entry->idx == reference && entry->key == ZEPHYRBT_BLACKBOARD_ITEM_ENTRY_KEY) {
			return entry;
		}
	}

	return NULL;
}

inline enum zephyrbt_child_status zephyrbt_evaluate(struct zephyrbt_context *ctx,
						    struct zephyrbt_node *self)
{
#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO)
	LOG_DBG("eval %s", self->name);
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
		LOG_ERR("The behaviour tree context is invalid. Thread aborted!");
		return;
	}

	if (ctx->node == NULL) {
		LOG_ERR("The behaviour tree API is invalid. Thread aborted!");
		return;
	}

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT)
	/*
	 * Initialization must be in the reverse order to ensure that
	 * dependencies will be initialized first. This allow a global
	 * context to be initialized at root or as close as root possible.
	 */
	for (int i = ctx->nodes - 1; i >= 0; --i) {
		struct zephyrbt_node *self = zephyrbt_get_node(ctx, i);

		if (self == NULL) {
			LOG_ERR("Node %d is invalid", i);
			continue;
		}

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT)
		self->ctx = NULL;
#endif

		if (self->init == NULL) {
			continue;
		}

		self->init(ctx, self);
	}
#endif

	while (true) {
		LOG_DBG("tick");
		zephyrbt_evaluate(ctx, zephyrbt_get_root(ctx));

#if defined(CONFIG_ZEPHYR_BEHAVIOUR_TREE_ALLOW_YIELD)
		if (ctx->thread_yield) {
			k_yield();
		}
#endif
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
