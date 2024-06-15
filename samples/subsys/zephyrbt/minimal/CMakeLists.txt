# Copyright (c) 2024 O.S. Systems Software LTDA.
# Copyright (c) 2024 Freedom Veiculos Eletricos
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyrbt_minimal)

zephyr_include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(app PRIVATE
	src/main.c
	src/zbt_minimal.c
)
