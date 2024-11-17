.. Copyright (c) 2024 O.S. Systems Software LTDA.
.. Copyright (c) 2024 Freedom Veiculos Eletricos
.. SPDX-License-Identifier: Apache-2.0
.. _zephyrbt_tutorial_lesson_1:

Zephyr Behaviour Tree - Tutorial - Lesson 2
###########################################

The Action initialization function
**********************************

In this example the counter will be moved to the Node Context. The Node Context
is a special place that can be used to store user data. The data is stored by
Node Instance. This means that if an Action is used multiple times in the tree,
each individual node will have their own context. In this case, it is the user
that defines how this will be implemented.

1- Override the zephyrbt_action_grab_beer_init function on the grab_beer.c
file. The <function>_init signature is executed only once when the ZephyrBT
thread is started. The goal of this function is to provide a way to initialize
the Action node.

.. code-block:: c

   struct zephyrbt_action_grab_beer_context {
      int counter;
   };

   enum zephyrbt_child_status
   zephyrbt_action_grab_beer_init(struct zephyrbt_context *ctx,
                  struct zephyrbt_node *self)
   {
      LOG_DBG("zephyrbt_action_grab_beer_init stub function");

      struct zephyrbt_action_grab_beer_context *grab_beeer_ctx;

      grab_beeer_ctx = k_malloc(sizeof(struct zephyrbt_action_grab_beer_context));
      self->ctx = grab_beeer_ctx;

      if (grab_beeer_ctx == NULL) {
         LOG_ERR("Context can not be allocate. Need more memory!!");
         return ZEPHYRBT_CHILD_FAILURE_STATUS;
      }

      memset(grab_beeer_ctx, 0, sizeof(struct zephyrbt_action_grab_beer_context));

      return ZEPHYRBT_CHILD_SUCCESS_STATUS;
   }

At this stage it is possible to understand that there are multiple contexts.
Each node can have their own and there is a centralized tree context. The tree
context will be explored later.

2- Update the zephyrbt_action_grab_beer action

.. code-block:: c

   enum zephyrbt_child_status
   zephyrbt_action_grab_beer(struct zephyrbt_context *ctx,
            struct zephyrbt_node *self)
   {
      struct zephyrbt_action_grab_beer_context *grab_beeer_ctx;
      grab_beeer_ctx = (struct zephyrbt_action_grab_beer_context *)self->ctx;

      ++grab_beeer_ctx->counter;

      LOG_DBG("\nGrab Beer try: %d\n", grab_beeer_ctx->counter);

      if (grab_beeer_ctx->counter < 3) {
         return ZEPHYRBT_CHILD_SUCCESS_STATUS;
      }

      return ZEPHYRBT_CHILD_FAILURE_STATUS;
   }

3- Try again with a Action Context

At this moment you have created an Action node that can be used multiple times
in your Behaviour Tree. In the first example the shared counter would be used by
all the instances. In the ZephyrBT each tree uses their own thread. Since the
Behaviour Tree execution is well defined there is no issues about shared
resources. It is not recommended that user create their own thread inside a
node. There are other ways to communicate between process and this topic will be
explored in future examples.

4- Try to change the tree and add another Grab Beer node.

It could be interesting to explorer using Zephyr Logs. Remember that the
behaviour of the action is the same, unless some random() actions take place.

5- Explore the initialization on other nodes

You can now explore more initialization functions on other nodes and expand to
your own super let's drink app. One idea is to use GPIOs and LEDs to have more
feedback. The easy way to start is to look on the code generated to grab the
functions signature and the enumeration. All the generated files are save by the
generation script on the place you define.

The ``zephyrbt_define_from_behaviourtreecpp_xml`` cmake function have the
following parameters:

.. code-block:: cmake

    target      # The current target used to add the generated files
    input_file  # The behaviour tree input file
    output_inc  # Output directory of the generated header
    output_src  # Output directory of the generated sources
    stack_size  # The amount of RAM used to run the BT
    thread_prio # The Thread Priority

This function define that the groot2 xml file format will be used as behaviour
tree input. Then the ``output_inc`` and ``output_src`` places are used to save
the generated files. In this tutorial we used the cmake default ``binary``
folder.

.. code-block:: cmake

        zephyrbt_define_from_behaviourtreecpp_xml(app
                models/tutorial.xml
                ${CMAKE_BINARY_DIR}/include
                ${CMAKE_BINARY_DIR}/src
                1024
                0
        )

Each behaviour tree like ``<my tree>.xml`` will generate an include file named
``<my tree>.h`` and two source files ``<my tree>_data.xml`` and
``<my tree>_stub.xml``. This information is create for any user defined node.

The include files will contain all the functions signatures and the enumeration
related to some node. The enumeration is create based on the ports that user
define.

The data file contain three definitions:

  * The ``zephyrbt_node`` vector structure with the behaviour tree
  * The ``zephyrbt_blackboard_item`` vector structure with the blackboard
  * And define the behaviour tree thread.

The stub file define all the skeletons to allow the program compile. This allows
the developer define their actions and at same time run the program.
