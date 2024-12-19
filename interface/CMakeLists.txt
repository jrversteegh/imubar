# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Default board to seeed xiao esp32s3
if (NOT DEFINED ${BOARD})
  set(BOARD xiao_esp32s3/esp32s3/procpu)
endif()

# Define board root to find our custom shield
set(BOARD_ROOT ${CMAKE_SOURCE_DIR})

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(imubar)

FILE(GLOB app_sources src/fonts/*.c src/*.cpp)
set(common_sources ../../src/interface.cpp)

add_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)

target_sources(app PRIVATE ${app_sources} ${common_sources})
add_compile_definitions(INTERFACE_UART=xiao_serial)
