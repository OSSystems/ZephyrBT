.. Copyright (c) 2024 O.S. Systems Software LTDA.
.. Copyright (c) 2024 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0
.. _zephyrbt_dynamic:

Zephyr Behaviour Tree - Dynamic
###############################

Overview
********

The Dynamic sample.

Building and Running
********************

This application can be built and executed on ``native_posix_64`` as follows:

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/zephyrbt/dynamic
   :host-os: unix
   :board: native_posix_64
   :goals: run
   :compact:

To build for another board, change "native_posix_64" above to that board's name.

Sample Output
=============

.. code-block:: console

   [2/130] Generating include/generated/version.h
   -- Zephyr version: 3.6.99 (/home/gfbudke/workspace/freedom/deps/zephyr), build: v3.6.0-2217-gd3a962733227
   [13/130] Generating include/lc_rs_bt.h, src/lc_rs_bt_data.c, src/lc_rs_bt_stub.c
   -- Behaviour Tree:  lc_rs_bt
   -- Stack Size:      1024
   -- Thread Priority: 0
   [23/130] Generating /home/gfbudke/workspace/freedom/deps/modules/zep...s/modules/zephyrbt/samples/subsys/zephyrbt/dynamic/src/modelo_stub.c
   -- Behaviour Tree:  modelo
   -- Stack Size:      1024
   -- Thread Priority: 0
   [129/130] Linking C executable zephyr/zephyr.elf
   Generating files from /home/gfbudke/workspace/freedom/build/zephyr/zephyr.elf for board: native_posix
   [129/130] cd /home/gfbudke/workspace/freedom/build && /home/gfbudke/workspace/freedom/build/zephyr/zephyr.exe
   uart connected to pseudotty: /dev/pts/1
   *** Booting Zephyr OS build v3.6.0-2217-gd3a962733227 ***
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: A [control, 12]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: A [control, 12]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: B [control, 11]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: B [control, 11]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: F [control, 10]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: F [control, 10]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: I [action, 9]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: I [action, 9]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: J [action, 8]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: J [action, 8]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: K [action, 7]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: K [action, 7]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: G [action, 6]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: G [action, 6]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: C [action, 5]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: C [action, 5]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: D [control, 4]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: D [control, 4]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: H [control, 3]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: H [control, 3]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: L [action, 2]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: L [action, 2]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: M [action, 1]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: M [action, 1]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: E [action, 0]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:00.000,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: E [action, 0]
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_action_ask_for_help_init stub function
   [00:00:00.000,000] <dbg> zephyrbt_decorator_timeout: zephyrbt_decorator_timeout_init: timeout [decorator, 1]
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_action_place_ball_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_placed_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_action_approch_bin_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_bin_close_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_action_grasp_ball_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_grasped_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_action_approch_ball_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_close_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_action_find_ball_init stub function
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_found_init stub function
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 18]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 18]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: sequence [control, 17]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: sequence [control, 17]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 16]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 16]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_found [condition, 15]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_found stub function
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 13]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 13]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_close [condition, 12]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_close stub function
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 10]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 10]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_grasped [condition, 9]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_grasped stub function
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 7]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 7]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: bin_close [condition, 6]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_bin_close stub function
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 4]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:00.000,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 4]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_placed [condition, 3]
   [00:00:00.000,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:00.000,000] <wrn> modelo_stub: zephyrbt_condition_ball_placed stub function
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: A [control, 12]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: A [control, 12]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: B [control, 11]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: B [control, 11]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: F [control, 10]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: F [control, 10]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: I [action, 9]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: I [action, 9]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: J [action, 8]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: J [action, 8]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: K [action, 7]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: K [action, 7]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: G [action, 6]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: G [action, 6]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: C [action, 5]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: C [action, 5]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: D [control, 4]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: D [control, 4]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: H [control, 3]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: H [control, 3]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: L [action, 2]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: L [action, 2]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: M [action, 1]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: M [action, 1]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: E [action, 0]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:01.010,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: E [action, 0]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 18]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 18]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: sequence [control, 17]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: sequence [control, 17]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 16]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 16]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_found [condition, 15]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <wrn> modelo_stub: zephyrbt_condition_ball_found stub function
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 13]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 13]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_close [condition, 12]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <wrn> modelo_stub: zephyrbt_condition_ball_close stub function
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 10]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 10]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_grasped [condition, 9]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <wrn> modelo_stub: zephyrbt_condition_ball_grasped stub function
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 7]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 7]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: bin_close [condition, 6]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <wrn> modelo_stub: zephyrbt_condition_bin_close stub function
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 4]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:01.010,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 4]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_placed [condition, 3]
   [00:00:01.010,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:01.010,000] <wrn> modelo_stub: zephyrbt_condition_ball_placed stub function
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: A [control, 12]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: A [control, 12]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: B [control, 11]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: B [control, 11]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: F [control, 10]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: F [control, 10]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: I [action, 9]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: I [action, 9]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: J [action, 8]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: J [action, 8]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: K [action, 7]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: K [action, 7]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: G [action, 6]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: G [action, 6]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: C [action, 5]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: C [action, 5]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: D [control, 4]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: D [control, 4]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: H [control, 3]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: H [control, 3]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: L [action, 2]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: L [action, 2]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: M [action, 1]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: M [action, 1]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: E [action, 0]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:02.020,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: E [action, 0]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 18]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 18]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: sequence [control, 17]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: sequence [control, 17]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 16]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 16]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_found [condition, 15]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <wrn> modelo_stub: zephyrbt_condition_ball_found stub function
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 13]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 13]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_close [condition, 12]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <wrn> modelo_stub: zephyrbt_condition_ball_close stub function
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 10]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 10]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_grasped [condition, 9]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <wrn> modelo_stub: zephyrbt_condition_ball_grasped stub function
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 7]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 7]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: bin_close [condition, 6]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <wrn> modelo_stub: zephyrbt_condition_bin_close stub function
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 4]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:02.020,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 4]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_placed [condition, 3]
   [00:00:02.020,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:02.020,000] <wrn> modelo_stub: zephyrbt_condition_ball_placed stub function
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: A [control, 12]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: A [control, 12]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: B [control, 11]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: B [control, 11]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: F [control, 10]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: F [control, 10]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: I [action, 9]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: I [action, 9]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: J [action, 8]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: J [action, 8]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: K [action, 7]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: K [action, 7]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: G [action, 6]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: G [action, 6]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: C [action, 5]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: C [action, 5]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: D [control, 4]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: D [control, 4]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: H [control, 3]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: H [control, 3]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: L [action, 2]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: L [action, 2]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: M [action, 1]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: M [action, 1]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: E [action, 0]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:03.030,000] <dbg> zephyrbt_actions: zephyrbt_action_always_success: E [action, 0]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_thread_func: tick
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 18]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 1
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 18]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: sequence [control, 17]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 2
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_sequence: sequence [control, 17]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: fallback [control, 16]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 3
   [00:00:03.030,000] <dbg> zephyrbt_controls: zephyrbt_control_fallback: fallback [control, 16]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: ball_found [condition, 15]
   [00:00:03.030,000] <dbg> zephyrbt: zephyrbt_evaluate: Deep: 4
