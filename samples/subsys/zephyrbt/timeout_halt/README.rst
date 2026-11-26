.. _zephyrbt_timeout_halt:

Zephyr Behaviour Tree - Timeout Halt
####################################

Overview
********

This sample is a regression test for the ``Timeout`` decorator. The tree
leaves a running ``Timeout`` and comes back to it later. The ``Timeout``
must then start a new deadline.

ZephyrBT evaluates the tree again from the root on each tick. When an earlier
node fails, the tree does not evaluate the ``Timeout`` on that tick, and the
``Sequence`` halts it. The halt function of the ``Timeout`` stops the timer, as
``TimeoutNode::halt()`` does in BehaviorTree.CPP.

Timeline:

#. The gate is open for 50 ms. ``Work`` runs inside the ``Timeout`` (100 ms).
#. The gate is closed until 200 ms. The tree runs ``Idle``. The deadline of
   the first run expires in this period.
#. The gate opens again. ``Work`` runs inside the ``Timeout`` again.
#. When the ``Timeout`` fails, ``ReportTimeout`` logs how long the second run
   took.

Building and Running
********************

.. code-block:: console

   west build -b qemu_cortex_m3 samples/subsys/zephyrbt/timeout_halt
   west build -t run

Sample Output
=============

.. code-block:: console

   I: Timeout restarted: 100 ms

Without the fix, the second run fails at once with the deadline of the first
run:

.. code-block:: console

   E: Timeout kept the stale deadline: 0 ms
