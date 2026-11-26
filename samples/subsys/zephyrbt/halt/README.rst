.. _zephyrbt_halt:

Zephyr Behaviour Tree - Halt
############################

Overview
********

The tree is evaluated again from the root on each tick. A node that returned
``RUNNING`` can thus be left by the tree. The tree then halts the node, as
``halt()`` does in BehaviorTree.CPP: it calls the halt function of each
running node of the subtree, from the leaves to the node, and runs the Lua
``_onHalted`` condition of these nodes.

The generator adds a weak halt function for each user node,
``zephyrbt_<type>_<name>_halt()``. This sample overrides
``zephyrbt_action_work_halt()``.

Timeline:

#. ``GateOpen`` succeeds for 3 ticks. ``Work`` runs inside a ``Timeout``.
#. ``GateOpen`` fails. The ``Sequence`` halts the ``Timeout``, and the
   ``Timeout`` halts ``Work`` first. The halt function of ``Work`` counts the
   halt in ``work_halts``. Then ``_onHalted`` of the ``Timeout`` copies
   ``work_halts`` to ``order``.
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
