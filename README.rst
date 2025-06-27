.. Copyright (c) 2024-2025 O.S. Systems Software LTDA.
.. Copyright (c) 2024-2025 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0

.. _zephyr_behaviour_tree_module:

Zephyr Behaviour Tree
#####################

This repository is the Zephyr Behaviour Tree module.

Preparation
***********

The first step is prepare the development machine. The developer must follow the
steps at `Zephyr RTOS getting started`_. The recommendation is test using the
``qemu_cortex_m3`` board. In this case, the last step from procedure should try
using this command:

.. code-block:: console

  cd ~/zephyrproject/zephyr

.. code-block:: console

  west build -b qemu_cortex_m3 samples/hello_world -t run

The second step is add your credentials to github. This will add necessary
permissions to clone using ssh and send changes. The github documentation can
be accessed at `Connecting to GitHub with SSH`_.

.. _Zephyr RTOS getting started:
  https://docs.zephyrproject.org/latest/develop/getting_started/index.html

.. _Connecting to GitHub with SSH:
  https://docs.github.com/en/authentication/connecting-to-github-with-ssh

How to clone
************

First, create your ``root`` directory, for instance:

.. code-block:: console

  mkdir $HOME/zephyrbt

.. code-block:: console

  cd $HOME/zephyrbt


The next step is create a ``python virtual environment`` with the ``west`` tool:

.. code-block:: console

  python3 -m venv .venv

.. code-block:: console

  source .venv/bin/activate

.. code-block:: console

  pip install west

Then, initialize ``zephyrbt`` root folder:

.. code-block:: console

  west init -m git@github.com:ossystems/zephyrbt

.. code-block:: console

  west update

Make sure that all requirements are meet:

.. code-block:: console

  pip install -r deps/zephyr/scripts/requirements.txt

.. code-block:: console

  pip install -r zephyrbt/scripts/requirements.txt

Examples
********

* The `Tutorial`_ guide you step by step on ZephyrBT (comming soon).

* The `Minimal`_ is an example how to use the ZephyrBT without any IDE.

* The `Dynamic`_ uses their own thread and generate data and stubs from `Groot2`_ IDE.

.. _Tutorial:
  samples/subsys/zephyrbt/tutorial/README.rst

.. _Minimal:
  samples/subsys/zephyrbt/minimal/README.rst

.. _Dynamic:
  samples/subsys/zephyrbt/dynamic/README.rst

.. _Groot2:
  https://www.behaviortree.dev/groot/

Tests
*****

To execute the tests just run twister pointing the TESTSUITE_ROOT to your
$HOME/zephyrbt folder.

.. code-block:: console

  west twister -T zephyrbt
