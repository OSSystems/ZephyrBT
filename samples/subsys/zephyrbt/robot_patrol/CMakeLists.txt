# Copyright (c) 2026 O.S. Systems Software LTDA.
# Copyright (c) 2026 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
include(${ZEPHYR_ZEPHYRBT_MODULE_DIR}/cmake/zephyrbt-from-behaviourtreecpp-xml.cmake)

project(zephyrbt_robot_patrol)

zephyr_include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(app PRIVATE
    src/main.c
    src/world.c
    src/navigation.c
    src/waypoint.c
    src/scan.c
    src/emergency.c
    src/charge.c
    src/lua_scripts.c
    src/display.c
)

if(NOT CONFIG_NATIVE_APPLICATION)
    target_sources(app PRIVATE src/posix_stubs.c)
endif()

zephyrbt_define_from_behaviourtreecpp_xml(app
    models/robot_patrol.xml
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/src
    8192
    0
    yes
)

# User Lua files (strong overrides, loaded before generated).
# diagnostics.lua is loaded first since others depend on manhattan().
zephyrbt_add_lua_file(app lua/diagnostics.lua)
zephyrbt_add_lua_file(app lua/navigation.lua)
zephyrbt_add_lua_file(app lua/safety.lua)
zephyrbt_add_lua_file(app lua/mission.lua)
