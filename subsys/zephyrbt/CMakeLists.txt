# Copyright (c) 2024-2026 O.S. Systems Software LTDA.
# Copyright (c) 2024-2026 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

zephyr_sources_ifdef(CONFIG_ZEPHYR_BEHAVIOUR_TREE
	zephyrbt.c
	zephyrbt_actions.c
	zephyrbt_controls.c
	zephyrbt_decorator_inverter.c
)

zephyr_sources_ifdef(CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT
	zephyrbt_action_set_blackboard.c
	zephyrbt_action_sleep.c
	zephyrbt_decorator_delay.c
	zephyrbt_decorator_run_once.c
	zephyrbt_decorator_timeout.c
)

zephyr_sources_ifdef(CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS
	zephyrbt_lua.c
)

# Support LD linker template
zephyr_linker_sources(DATA_SECTIONS zephyrbt.ld)

# Support CMake linker generator
zephyr_iterable_section(
  NAME zephyr_behaviour_tree
  GROUP DATA_REGION ${XIP_ALIGN_WITH_INPUT})
