.. Copyright (c) 2024 O.S. Systems Software LTDA.
.. Copyright (c) 2024 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0
.. _zephyrbt_minimal:

Zephyr Behaviour Tree - Minimal
###############################

Overview
********

The Minimal sample.

Building and Running
********************

This application can be built and executed on ``native_posix_64`` as follows:

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/zephyrbt/minimal
   :host-os: unix
   :board: native_posix_64
   :goals: run
   :compact:

To build for another board, change "native_posix_64" above to that board's name.

Sample Output
=============

.. code-block:: console

   [00:00:00.000,000] <dbg> lc_rs_bt_data: lc_rs_bt_thread: tick
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
