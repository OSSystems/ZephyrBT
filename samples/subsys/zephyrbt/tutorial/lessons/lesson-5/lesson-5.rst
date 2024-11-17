.. Copyright (c) 2024 O.S. Systems Software LTDA.
.. Copyright (c) 2024 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0
.. _zephyrbt_tutorial_lesson_1:

Zephyr Behaviour Tree - Tutorial - Lesson 5
###########################################

Using ZBUS
**********

1 - In this instruction will be necessary to use some board. The content was
developed using the ST Nucleo F767-ZI.

In this case, to build you need to change your command to:

.. code-block:: console

   west build -p -b nucleo_f767zi samples/subsys/zephyrbt/tutorial

.. code-block:: console

   west flash [-r openocd]

2- This example consist in turn a led On/Off depending on the fridge door state.
To copy the files, navigate to ``samples/subsys/zephyrbt/tutorial`` folder and
execute the following command.

.. code-block:: console

   cp -r lessons/lesson-5/resources/* .

In the boards folder a new configuration was created to enable PWM leds. In the
``zbus_peripherals.h`` file it is defined the structs to handle the peripherals
states. In this case, the ``ZBUS_PERIPHERAL_WRITE`` abstracts the peripherals
and update the state in case of changes. The ``zbus_peripherals.c`` is given to
fullfil the purpose of the sample. It defines a channel and the peripherals
thread. With those files it is possible to update the fridge.c file.

.. code-block:: c

   /* Add the new two include files */
   #include <zephyr/zbus/zbus.h>
   #include "zbus_peripherals.h"

   /* Declare the channel to allow send data */
   ZBUS_CHAN_DECLARE(peripherals_channel);

   /* Use the channel */
   ZBUS_PERIPHERAL_WRITE(.led_green.state = LEDS_STATES_ON);

   /* Multiple peripherals can be updated at same time */
   ZBUS_PERIPHERAL_WRITE(
        .led_green.state = LEDS_STATES_ON,
        .led_red.state = LEDS_STATES_OFF
   );
