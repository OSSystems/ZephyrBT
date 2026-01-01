.. Copyright (c) 2026 O.S. Systems Software LTDA.
.. Copyright (c) 2026 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0
.. _zephyrbt_subtree_reuse:

Zephyr Behaviour Tree - Subtree Reuse
#####################################

Overview
********

This sample validates the correct behavior tree traversal when a subtree is
used multiple times. It reproduces the scenario from `GitHub issue #28`_ where
incorrect indices caused nodes to be skipped during execution.

The behavior tree structure is:

.. code-block:: text

   Sequence
   ├── SubTree (btree_subtree) → sub
   ├── A
   ├── SubTree (btree_subtree) → sub
   └── B

The expected execution order is: sub → A → sub → B

Prior to the fix, the second instance of the subtree had incorrect sibling
indices, causing node A to be skipped entirely.

.. _GitHub issue #28: https://github.com/OSSystems/ZephyrBT/issues/28

Building and Running
********************

This application can be built and executed on ``qemu_cortex_m3`` as follows:

.. code-block:: console

   west build -p -b qemu_cortex_m3 samples/subsys/zephyrbt/subtree_reuse -t run

To build for another board, change "qemu_cortex_m3" above to that board's name.

Sample Output
=============

.. code-block:: console

   *** Booting Zephyr OS ***
   D: zephyrbt_action_sub_init stub function
   D: zephyrbt_action_a_init stub function
   D: zephyrbt_action_sub_init stub function
   D: zephyrbt_action_b_init stub function
   D: tick
   D: eval sequence [control, 4]
   D: Deep: 1
   D: sequence [control, 4]
   D: eval sub [action, 3]
   D: Deep: 2
   D: zephyrbt_action_sub stub function
   D: eval a [action, 2]
   D: Deep: 2
   D: zephyrbt_action_a stub function
   D: eval sub [action, 1]
   D: Deep: 2
   D: zephyrbt_action_sub stub function
   D: eval b [action, 0]
   D: Deep: 2
   D: zephyrbt_action_b stub function

The key verification points are:

1. All four nodes (sub, a, sub, b) are evaluated in the correct order
2. Node 'a' is not skipped (which was the bug symptom)
3. Each subtree instance has a unique index (3 and 1)
