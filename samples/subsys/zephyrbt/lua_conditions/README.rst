.. Copyright (c) 2026 O.S. Systems Software LTDA.
.. Copyright (c) 2026 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0
.. _zephyrbt_lua_conditions:

Zephyr Behaviour Tree - Lua Conditions
#######################################

Overview
********

This sample demonstrates pre/post conditions on behaviour tree nodes
using Lua scripts. Conditions are defined as XML attributes and
evaluated inline during tree evaluation.

The sample exercises these condition types:

- ``_skipIf`` — skip node when counter exceeds a threshold
- ``_successIf`` — short-circuit to SUCCESS on even counter values
- ``_failureIf`` — return FAILURE when counter reaches a specific value
- ``_onSuccess`` / ``_onFailure`` — set blackboard flags after tick
- ``_post`` — increment a counter after every tick

Each condition is generated as a weak Lua function in
``lua_conditions_gen.lua``. Users can override any function by providing
a ``.lua`` file loaded before the generated one (see `Overriding
Conditions`_ below).

Prerequisites
*************

This sample requires the ``lua_zephyr`` module. After cloning ZephyrBT,
fetch all dependencies and initialize the Lua submodule:

.. code-block:: console

   west update
   cd deps/modules/lib/lua_zephyr
   git submodule update --init
   cd -

The ``west update`` command fetches ``lua_zephyr`` and other west
projects. The ``git submodule update --init`` step inside ``lua_zephyr``
clones the Lua 5.5.0 core sources which are tracked as a git submodule.

Building and Running
********************

This application can be built and executed on ``native_sim/native/64``
as follows:

.. code-block:: console

   west build -p -b native_sim/native/64 samples/subsys/zephyrbt/lua_conditions -t run

For embedded targets with sufficient RAM (e.g. ``mps2/an385``):

.. code-block:: console

   west build -p -b mps2/an385 samples/subsys/zephyrbt/lua_conditions -t run

.. note::

   The ``qemu_cortex_m3`` board has only 64 KB of RAM which is not
   sufficient for Lua. Use ``native_sim/native/64`` or a board with at
   least 256 KB of RAM.

Sample Output
=============

.. code-block:: console

   *** Booting Zephyr OS build 26d4afe225d0 ***
   I: increment: counter=1
   I: check_value: ticked (odd counter)
   I: guarded_action: executed
   I: increment: counter=2
   I: guarded_action: executed
   I: increment: counter=3
   I: check_value: ticked (odd counter)
   I: guarded_action: executed
   I: increment: counter=4
   I: guarded_action: executed
   I: guarded_action: executed

Observations:

- ``increment`` stops appearing after ``counter=4`` because ``_skipIf``
  triggers (``bb.counter > 3``).
- ``check_value: ticked`` only appears for odd counters (1, 3). Even
  values (2, 4) are short-circuited by ``_successIf``
  (``bb.counter % 2 == 0``).
- ``guarded_action`` continues running because the counter stays at 4
  and ``_failureIf`` checks ``bb.counter == 5``.

Generated Lua File
==================

The code generator produces ``lua_conditions_gen.lua`` from the XML
condition attributes. Each condition becomes a weak-guarded function:

.. code-block:: lua

   if not inc_counter_skip_if then
   function inc_counter_skip_if(bb)
       return bb.counter > 3
   end
   end

   if not inc_counter_post then
   function inc_counter_post(bb)
       bb.ticks = bb.ticks + 1
   end
   end

Functions receive the blackboard (``bb``) as an argument. Read values
with ``bb.name`` and write with ``bb.name = value``.

Overriding Conditions
=====================

To override a generated condition, create a ``.lua`` file with the same
function name and register it in ``CMakeLists.txt``:

.. code-block:: lua

   -- src/my_overrides.lua
   function inc_counter_skip_if(bb)
       return bb.counter > 10
   end

.. code-block:: cmake

   zephyrbt_add_lua_file(app src/my_overrides.lua)

User files are loaded before the generated script. Since generated
functions use ``if not ... then`` guards, user definitions take
precedence.
