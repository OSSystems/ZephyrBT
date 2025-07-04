# Copyright (c) 2024-2025 O.S. Systems Software LTDA.
# Copyright (c) 2024-2025 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
include(${ZEPHYR_ZEPHYRBT_MODULE_DIR}/cmake/zephyrbt-from-behaviourtreecpp-xml.cmake)

project(zephyrbt_tutorial)

zephyr_include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(app PRIVATE
	src/main.c
	src/fridge.c
	src/global_ctx.c
	src/grab_beer.c
)

zephyrbt_define_from_behaviourtreecpp_xml(app
	models/tutorial.xml
	${CMAKE_BINARY_DIR}/include
	${CMAKE_BINARY_DIR}/src
	1024
	0
	yes
)
