# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)


#set(DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/eink_companion_nrf52840dk.overlay")
set(DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/eink_companion_nrf52810_custom.overlay")


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(spi_bitbang)


FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
