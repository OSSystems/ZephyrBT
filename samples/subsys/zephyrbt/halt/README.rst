.. _zephyrbt_halt:

Zephyr Behaviour Tree - Halt
############################

Overview
********

The tree is evaluated again from the root on each tick. A node that returned
``RUNNING`` can thus be left by the tree. The tree then halts the node, as
``halt()`` does in BehaviorTree.CPP: it calls the halt function of the node
and then halts the running nodes of its subtree in the same way. The Lua
``_onHalted`` condition of a node runs after its subtree halted.

The generator adds a weak halt function for each user node,
``zephyrbt_<type>_<name>_halt()``. This sample overrides
``zephyrbt_action_work_halt()``.

Timeline:

#. ``GateOpen`` succeeds for 3 ticks. ``Work`` runs inside a ``Timeout``.
#. ``GateOpen`` fails. The ``Sequence`` halts the ``Timeout``: the
   ``Timeout`` stops its timer, and then ``Work`` halts. The halt function of
   ``Work`` counts the halt in ``work_halts``. Then ``_onHalted`` of the
   ``Timeout`` copies ``work_halts`` to ``order``.
#. After 3 more ticks, ``Report`` logs both values.

Building and Running
********************

.. code-block:: console

   west build -b mps2/an385 samples/subsys/zephyrbt/halt
   west build -t run

Sample Output
=============

.. code-block:: console

   I: halt result: work=1 order=1
