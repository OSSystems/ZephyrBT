# Copyright (c) 2024-2026 O.S. Systems Software LTDA.
# Copyright (c) 2024-2026 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

include(CheckIncludeFile)

function(zephyrbt_define_from_behaviourtreecpp_xml
    target       # The current target used to add the generated files
    input_file   # The behaviour tree input file
    output_inc   # Output directory of the generated header
    output_src   # Output directory of the generated sources
    stack_size   # The amount of RAM used to run the BT
    thread_prio  # The Thread Priority
    thread_yield # The Thread yield option [ True or False ]
    )
  get_filename_component(zephyrbt_name ${input_file} NAME_WE [CACHE])
  get_filename_component(input_file ${input_file} ABSOLUTE)
  get_filename_component(output_inc ${output_inc} ABSOLUTE)
  get_filename_component(output_src ${output_src} ABSOLUTE)

  set(zephyrbt_target    "${target}_${zephyrbt_name}")
  set(zephyrbt_inc_file  "${output_inc}/${zephyrbt_name}.h")
  set(zephyrbt_data_file "${output_src}/${zephyrbt_name}_data.c")
  set(zephyrbt_stub_file "${output_src}/${zephyrbt_name}_stub.c")
  set(zephyrbt_lua_file  "${output_src}/${zephyrbt_name}_gen.lua")

  file(GLOB_RECURSE zephyrbt_user_include ${CMAKE_CURRENT_SOURCE_DIR}/include/zephyrbt_user.h)
  string(LENGTH "${zephyrbt_user_include}" zephyrbt_user_include_valid)

  if(${zephyrbt_user_include_valid} GREATER 0)
    set(zephyrbt_user_include_file "-u True")
  else()
    set(zephyrbt_user_include_file "")
  endif()

  if(${CONFIG_ZEPHYR_BEHAVIOUR_TREE_ALLOW_YIELD})
    if(${thread_yield})
      set(zephyrbt_thread_yield "-y 1")
    else()
      set(zephyrbt_thread_yield "-y 0")
    endif()
  else()
    set(zephyrbt_thread_yield "-y 2")
  endif()

  add_custom_command(
    OUTPUT  ${zephyrbt_inc_file}
            ${zephyrbt_data_file}
            ${zephyrbt_stub_file}
            ${zephyrbt_lua_file}
    DEPENDS ${input_file}
    COMMAND ${PYTHON_EXECUTABLE}
      ${ZEPHYR_ZEPHYRBT_MODULE_DIR}/scripts/generate-zephyrbt-from-behaviourtreecpp-xml
      -i  ${input_file}
      -oi ${zephyrbt_inc_file}
      -os ${zephyrbt_data_file}
      -ot ${zephyrbt_stub_file}
      -s  ${stack_size}
      -p  ${thread_prio}
      ${zephyrbt_thread_yield}
      ${zephyrbt_user_include_file}
  )

  # Embed the generated .lua file as a C string header
  # using lua_zephyr's luaz_gen.py script.
  if(CONFIG_ZEPHYR_BEHAVIOUR_TREE_LUA_CONDITIONS)
    set(zephyrbt_lua_hdr
        "${output_inc}/${zephyrbt_name}_gen_lua_script.h")
    set(luaz_gen_script
        "${ZEPHYR_LUA_MODULE_DIR}/scripts/luaz_gen.py")
    set(luaz_template
        "${ZEPHYR_LUA_MODULE_DIR}/templates/lua_template.h.in")

    add_custom_command(
      OUTPUT  ${zephyrbt_lua_hdr}
      DEPENDS ${zephyrbt_lua_file}
      COMMAND ${PYTHON_EXECUTABLE} ${luaz_gen_script}
        --mode source
        --template ${luaz_template}
        --output ${zephyrbt_lua_hdr}
        --name "${zephyrbt_name}_gen"
        ${zephyrbt_lua_file}
      COMMENT "Embedding ${zephyrbt_name}_gen.lua"
    )

    add_custom_target(${zephyrbt_target}_lua
      ALL
      DEPENDS ${zephyrbt_lua_hdr}
    )
    add_dependencies(${target} ${zephyrbt_target}_lua)
  endif()

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


# zephyrbt_add_lua_file(target, lua_file)
#
# Embed a user .lua file as a C string header for loading
# as a strong override before the generated weak conditions.
# The header provides const char {name}_lua_script[].
function(zephyrbt_add_lua_file target lua_file)
  cmake_path(GET lua_file FILENAME _fname)
  cmake_path(REMOVE_EXTENSION _fname OUTPUT_VARIABLE _name)

  set(_lua_src "${CMAKE_CURRENT_SOURCE_DIR}/${lua_file}")
  set(_lua_hdr
      "${CMAKE_CURRENT_BINARY_DIR}/include/${_name}_lua_script.h")
  set(luaz_gen_script
      "${ZEPHYR_LUA_MODULE_DIR}/scripts/luaz_gen.py")
  set(luaz_template
      "${ZEPHYR_LUA_MODULE_DIR}/templates/lua_template.h.in")

  add_custom_command(
    OUTPUT  ${_lua_hdr}
    DEPENDS ${_lua_src}
    COMMAND ${PYTHON_EXECUTABLE} ${luaz_gen_script}
      --mode source
      --template ${luaz_template}
      --output ${_lua_hdr}
      --name "${_name}"
      ${_lua_src}
    COMMENT "Embedding ${_name}.lua"
  )

  add_custom_target(${_name}_lua_header ALL
    DEPENDS ${_lua_hdr})
  add_dependencies(${target} ${_name}_lua_header)

  target_include_directories(${target} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include
  )
endfunction()
