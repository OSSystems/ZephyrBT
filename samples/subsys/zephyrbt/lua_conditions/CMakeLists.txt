# Copyright (c) 2026 O.S. Systems Software LTDA.
# Copyright (c) 2026 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
include(${ZEPHYR_ZEPHYRBT_MODULE_DIR}/cmake/zephyrbt-from-behaviourtreecpp-xml.cmake)

project(zephyrbt_lua_conditions)

zephyr_include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(app PRIVATE
    src/main.c
    src/lua_conditions.c
)

if(NOT CONFIG_NATIVE_APPLICATION)
    target_sources(app PRIVATE src/posix_stubs.c)
endif()

zephyrbt_define_from_behaviourtreecpp_xml(app
    models/lua_conditions.xml
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/src
    4096
    0
    yes
)
