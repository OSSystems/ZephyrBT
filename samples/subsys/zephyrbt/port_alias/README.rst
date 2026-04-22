.. Copyright (c) 2026 O.S. Systems Software LTDA.
.. Copyright (c) 2026 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0
.. _zephyrbt_port_alias:

Zephyr Behaviour Tree - Port Alias
##################################

Overview
********

This sample validates that multiple instances of the same node type with
different port bindings receive distinct blackboard references. It
reproduces the scenario from `GitHub issue #40`_ where the code
generator assigned all instances of a node the same blackboard ref due
to shared entry objects.

The behavior tree structure is:

.. code-block:: text

   Delay (1000 ms)
   └── Sequence
       ├── WriteValue → {variable_a}
       ├── WriteValue → {variable_b}
       ├── ReadValue  → {variable_a}
       └── ReadValue  → {variable_b}

Each ``WriteValue`` instance initialises its aliased blackboard entry
with a random seed and increments it on every tick. Each ``ReadValue``
instance logs the value it observes through its aliased entry.

Prior to the fix, both ``WriteValue`` nodes shared the same blackboard
ref, so ``variable_a`` and ``variable_b`` became aliases of a single
storage cell. With the fix, each node resolves to its own blackboard
item and the two values evolve independently.

The sample also exercises the ``CONFIG_ZEPHYR_BEHAVIOUR_TREE_NODE_CONTEXT``
optimisation: the ``_init`` callback caches the resolved blackboard item
pointer in ``self->ctx`` so the tick path avoids a runtime search.

.. _GitHub issue #40: https://github.com/OSSystems/ZephyrBT/issues/40

Building and Running
********************

This application can be built and executed on ``qemu_cortex_m3`` as
follows:

.. code-block:: console

   west build -p -b qemu_cortex_m3 samples/subsys/zephyrbt/port_alias -t run

To build for another board, change "qemu_cortex_m3" above to that
board's name.

Sample Output
=============

.. code-block:: console

   *** Booting Zephyr OS ***
   I: initial value [3]: wrote 1804289383
   I: initial value [2]: wrote 846930886
   I: write_value [3]: wrote 1804289384
   I: write_value [2]: wrote 846930887
   I: read_value [1]: read 1804289384
   I: read_value [0]: read 846930887
   I: write_value [3]: wrote 1804289385
   I: write_value [2]: wrote 846930888
   I: read_value [1]: read 1804289385
   I: read_value [0]: read 846930888

The key verification points are:

1. Each ``WriteValue`` node starts from a different random seed,
   confirming the two instances resolve to distinct blackboard items.
2. Per-index values are strictly increasing across ticks; no node
   overwrites another node's storage.
3. Each ``ReadValue`` observes the value written by the ``WriteValue``
   that shares its port alias (``variable_a`` or ``variable_b``).

A Twister ``pytest`` harness at ``pytest/test_port_alias.py`` asserts
these properties automatically on ``qemu_cortex_m3``.
