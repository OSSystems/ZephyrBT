/*
 * Copyright (c) 2026 O.S. Systems Software LTDA.
 * Copyright (c) 2026 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/logging/log.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

LOG_MODULE_DECLARE(zephyrbt, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

static void *zephyrbt_lua_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	ARG_UNUSED(ud);
	ARG_UNUSED(osize);

	if (nsize == 0) {
		k_free(ptr);
		return NULL;
	}

	if (ptr == NULL) {
		return k_malloc(nsize);
	}

	void *new_ptr = k_malloc(nsize);

	if (new_ptr != NULL && ptr != NULL) {
		size_t copy_size = osize < nsize ? osize : nsize;

		memcpy(new_ptr, ptr, copy_size);
		k_free(ptr);
	}

	return new_ptr;
}

/*
 * Blackboard bridge: "bb" userdata with __index/__newindex
 * metamethods. Stored as a Lua registry ref and passed as
 * argument to condition functions (not a global).
 */

static struct zephyrbt_context *bb_get_ctx(lua_State *L)
{
	struct zephyrbt_context **ud = (struct zephyrbt_context **)lua_touserdata(L, 1);
	return *ud;
}

static struct zephyrbt_blackboard_item *bb_find(struct zephyrbt_context *ctx, const char *name)
{
	for (int i = 0; i < ctx->bb_names_count; i++) {
		if (strcmp(ctx->bb_names[i].name, name) == 0) {
			int bb_idx = ctx->bb_names[i].bb_idx;
			struct zephyrbt_blackboard_item *entry = ctx->blackboard;

			for (; entry->idx != -1; entry++) {
				if (entry->idx == bb_idx &&
				    entry->key == ZEPHYRBT_BLACKBOARD_ITEM_ENTRY_KEY) {
					return entry;
				}
			}
		}
	}

	return NULL;
}

static int bb_index(lua_State *L)
{
	struct zephyrbt_context *ctx = bb_get_ctx(L);
	const char *key = luaL_checkstring(L, 2);
	struct zephyrbt_blackboard_item *item = bb_find(ctx, key);

	if (item == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushinteger(L, (lua_Integer)(intptr_t)item->item);
	return 1;
}

static int bb_newindex(lua_State *L)
{
	struct zephyrbt_context *ctx = bb_get_ctx(L);
	const char *key = luaL_checkstring(L, 2);
	struct zephyrbt_blackboard_item *item = bb_find(ctx, key);

	if (item == NULL) {
		LOG_WRN("bb: unknown key '%s'", key);
		return 0;
	}

	if (lua_isboolean(L, 3)) {
		item->item = (zephyrbt_node_context_t)(intptr_t)lua_toboolean(L, 3);
	} else {
		item->item = (zephyrbt_node_context_t)(intptr_t)lua_tointeger(L, 3);
	}

	return 0;
}

static int zephyrbt_lua_create_bb(lua_State *L, struct zephyrbt_context *ctx)
{
	struct zephyrbt_context **ud =
		(struct zephyrbt_context **)lua_newuserdatauv(L, sizeof(void *), 0);
	*ud = ctx;

	luaL_newmetatable(L, "zephyrbt_bb");

	lua_pushcfunction(L, bb_index);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, bb_newindex);
	lua_setfield(L, -2, "__newindex");

	lua_setmetatable(L, -2);

	return luaL_ref(L, LUA_REGISTRYINDEX);
}

static bool zephyrbt_lua_eval_bool(struct zephyrbt_context *ctx, int ref)
{
	lua_State *L = ctx->lua;
	bool result;

	lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	lua_rawgeti(L, LUA_REGISTRYINDEX, ctx->bb_ref);
	if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
		LOG_ERR("Lua error: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	result = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return result;
}

static void zephyrbt_lua_exec(struct zephyrbt_context *ctx, int ref)
{
	lua_State *L = ctx->lua;

	lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	lua_rawgeti(L, LUA_REGISTRYINDEX, ctx->bb_ref);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
		LOG_ERR("Lua error: %s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

int zephyrbt_lua_init(struct zephyrbt_context *ctx)
{
	lua_State *L;

	L = lua_newstate(zephyrbt_lua_alloc, NULL, 0);
	if (L == NULL) {
		LOG_ERR("Failed to create Lua state");
		return -ENOMEM;
	}

	luaL_requiref(L, "_G", luaopen_base, 1);
	lua_pop(L, 1);
	luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
	lua_pop(L, 1);
	luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
	lua_pop(L, 1);

	ctx->bb_ref = zephyrbt_lua_create_bb(L, ctx);

	/* Load user scripts first (strong definitions) */
	for (int i = 0; i < ctx->lua_user_script_count; i++) {
		if (luaL_dostring(L, ctx->lua_user_scripts[i]) != LUA_OK) {
			LOG_ERR("Lua user script %d error: %s", i, lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	/* Load generated script last (weak definitions) */
	if (ctx->lua_gen_script != NULL) {
		if (luaL_dostring(L, ctx->lua_gen_script) != LUA_OK) {
			LOG_ERR("Lua gen script error: %s", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	/* Initialize all condition refs to LUA_NOREF */
	for (uint32_t i = 0; i < ctx->nodes; i++) {
		for (int j = 0; j < ZEPHYRBT_COND_COUNT; j++) {
			ctx->conditions[i].refs[j] = LUA_NOREF;
		}
	}

	/* Resolve function names to Lua registry refs */
	for (int i = 0; i < ctx->lua_cond_ref_count; i++) {
		const struct zephyrbt_lua_cond_ref *r = &ctx->lua_cond_refs[i];

		if (r->func_name == NULL) {
			continue;
		}

		lua_getglobal(L, r->func_name);
		if (!lua_isfunction(L, -1)) {
			LOG_WRN("Lua function '%s' not found", r->func_name);
			lua_pop(L, 1);
			continue;
		}

		int ref = luaL_ref(L, LUA_REGISTRYINDEX);

		ctx->conditions[r->node_idx].refs[r->cond_type] = ref;
	}

	ctx->lua = L;

	LOG_DBG("Lua initialized with %d conditions", ctx->lua_cond_ref_count);

	return 0;
}

bool zephyrbt_lua_while_check(struct zephyrbt_context *ctx, struct zephyrbt_node *self)
{
	struct zephyrbt_node_conditions *c = &ctx->conditions[self->index];

	if (c->refs[ZEPHYRBT_COND_WHILE] == LUA_NOREF) {
		return true;
	}

	return zephyrbt_lua_eval_bool(ctx, c->refs[ZEPHYRBT_COND_WHILE]);
}

enum zephyrbt_child_status zephyrbt_lua_pre_check(struct zephyrbt_context *ctx,
						  struct zephyrbt_node *self)
{
	struct zephyrbt_node_conditions *c = &ctx->conditions[self->index];

	if (c->refs[ZEPHYRBT_COND_FAILURE_IF] != LUA_NOREF) {
		if (zephyrbt_lua_eval_bool(ctx, c->refs[ZEPHYRBT_COND_FAILURE_IF])) {
			return ZEPHYRBT_CHILD_FAILURE_STATUS;
		}
	}

	if (c->refs[ZEPHYRBT_COND_SUCCESS_IF] != LUA_NOREF) {
		if (zephyrbt_lua_eval_bool(ctx, c->refs[ZEPHYRBT_COND_SUCCESS_IF])) {
			return ZEPHYRBT_CHILD_SUCCESS_STATUS;
		}
	}

	if (c->refs[ZEPHYRBT_COND_SKIP_IF] != LUA_NOREF) {
		if (zephyrbt_lua_eval_bool(ctx, c->refs[ZEPHYRBT_COND_SKIP_IF])) {
			return ZEPHYRBT_CHILD_SKIP_STATUS;
		}
	}

	if (c->refs[ZEPHYRBT_COND_WHILE] != LUA_NOREF) {
		if (!zephyrbt_lua_eval_bool(ctx, c->refs[ZEPHYRBT_COND_WHILE])) {
			return ZEPHYRBT_CHILD_SKIP_STATUS;
		}
	}

	return ZEPHYRBT_CHILD_RUNNING_STATUS;
}

void zephyrbt_lua_post_check(struct zephyrbt_context *ctx, struct zephyrbt_node *self,
			     enum zephyrbt_child_status status)
{
	struct zephyrbt_node_conditions *c = &ctx->conditions[self->index];

	if (status == ZEPHYRBT_CHILD_FAILURE_STATUS &&
	    c->refs[ZEPHYRBT_COND_ON_FAILURE] != LUA_NOREF) {
		zephyrbt_lua_exec(ctx, c->refs[ZEPHYRBT_COND_ON_FAILURE]);
	}

	if (status == ZEPHYRBT_CHILD_SUCCESS_STATUS &&
	    c->refs[ZEPHYRBT_COND_ON_SUCCESS] != LUA_NOREF) {
		zephyrbt_lua_exec(ctx, c->refs[ZEPHYRBT_COND_ON_SUCCESS]);
	}

	if (c->refs[ZEPHYRBT_COND_POST] != LUA_NOREF) {
		zephyrbt_lua_exec(ctx, c->refs[ZEPHYRBT_COND_POST]);
	}
}
