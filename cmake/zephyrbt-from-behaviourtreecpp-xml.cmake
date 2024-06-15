# Copyright (c) 2024 O.S. Systems Software LTDA.
# Copyright (c) 2024 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

include(CheckIncludeFile)

function(zephyrbt_define_from_behaviourtreecpp_xml
    target      # The current target used to add the generated files
    input_file  # The behaviour tree input file
    output_inc  # Output directory of the generated header
    output_src  # Output directory of the generated sources
    stack_size  # The amount of RAM used to run the BT
    thread_prio # The Thread Priority
    )
  get_filename_component(zephyrbt_name ${input_file} NAME_WE [CACHE])
  get_filename_component(input_file ${input_file} ABSOLUTE)
  get_filename_component(output_inc ${output_inc} ABSOLUTE)
  get_filename_component(output_src ${output_src} ABSOLUTE)

  set(zephyrbt_target    "${target}_${zephyrbt_name}")
  set(zephyrbt_inc_file  "${output_inc}/${zephyrbt_name}.h")
  set(zephyrbt_data_file "${output_src}/${zephyrbt_name}_data.c")
  set(zephyrbt_stub_file "${output_src}/${zephyrbt_name}_stub.c")

  file(GLOB_RECURSE zephyrbt_user_include zephyrbt_user.h)
  string(LENGTH "${zephyrbt_user_include}" zephyrbt_user_include_valid)

  if(${zephyrbt_user_include_valid} GREATER 0)
    set(zephyrbt_user_include_file "-u True")
  else()
    set(zephyrbt_user_include_file "")
  endif()

  add_custom_command(
    OUTPUT  ${zephyrbt_inc_file}
            ${zephyrbt_data_file}
            ${zephyrbt_stub_file}
    DEPENDS ${input_file}
    COMMAND ${PYTHON_EXECUTABLE}
      ${ZEPHYR_ZEPHYRBT_MODULE_DIR}/scripts/generate-zephyrbt-from-behaviourtreecpp-xml
      -i  ${input_file}
      -oi ${zephyrbt_inc_file}
      -os ${zephyrbt_data_file}
      -ot ${zephyrbt_stub_file}
      -s  ${stack_size}
      -p  ${thread_prio}
      ${zephyrbt_user_include_file}
  )

  add_custom_target(${zephyrbt_target}
    ALL
    DEPENDS ${input_file} ${zephyrbt_user_include}
  )

  target_include_directories(${target} PRIVATE
    ${output_inc}
  )

  target_sources(${target} PRIVATE
    ${zephyrbt_data_file}
    ${zephyrbt_stub_file}
  )
endfunction()
