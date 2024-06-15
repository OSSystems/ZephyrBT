/*
 * Copyright (c) 2024 O.S. Systems Software LTDA.
 * Copyright (c) 2024 Freedom Veiculos Eletricos
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zephyr/zephyrbt/zephyrbt.h>
#include <zephyr/sys/util.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zbt_minimal, CONFIG_ZEPHYR_BEHAVIOUR_TREE_LOG_LEVEL);

// clang-format off

static struct zephyrbt_node zbt_minimal_nodes[] = {
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=-1,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="E [action, 0]",
			.index=0,
		))
	},
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=-1,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="M [action, 1]",
			.index=1,
		))
	},
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=1,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="L [action, 2]",
			.index=2,
		))
	},
	{
		.function=zephyrbt_control_sequence,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=-1,
		.child=2,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="H [control, 3]",
			.index=3,
		))
	},
	{
		.function=zephyrbt_control_sequence,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=0,
		.child=3,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="D [control, 4]",
			.index=4,
		))
	},
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=4,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="C [action, 5]",
			.index=5,
		))
	},
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=-1,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="G [action, 6]",
			.index=6,
		))
	},
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=-1,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="K [action, 7]",
			.index=7,
		))
	},
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=7,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="J [action, 8]",
			.index=8,
		))
	},
	{
		.function=zephyrbt_action_always_success,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=8,
		.child=-1,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="I [action, 9]",
			.index=9,
		))
	},
	{
		.function=zephyrbt_control_sequence,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=6,
		.child=9,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="F [control, 10]",
			.index=10,
		))
	},
	{
		.function=zephyrbt_control_sequence,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=5,
		.child=10,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="B [control, 11]",
			.index=11,
		))
	},
	{
		.function=zephyrbt_control_sequence,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INIT, (
			.init=NULL,
		))
		.sibling=-1,
		.child=11,
		IF_ENABLED(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_INFO, (
			.name="A [control, 12]",
			.index=12,
		))
	},
};

static struct zephyrbt_context zbt_minimal_ctx = {
    .node       = zbt_minimal_nodes,
    .nodes      = ARRAY_SIZE(zbt_minimal_nodes),
    .blackboard = NULL,
};

// clang-format on

static void zbt_minimal_thread(void *, void *, void *)
{
	while (true) {
		LOG_DBG("tick");

		zephyrbt_evaluate(&zbt_minimal_ctx, zephyrbt_get_root(&zbt_minimal_ctx));

		k_sleep(K_MSEC(1000));
	}
}

K_THREAD_DEFINE(zbt_minimal, 512, zbt_minimal_thread, NULL, NULL, NULL, 0, 0, 0);
