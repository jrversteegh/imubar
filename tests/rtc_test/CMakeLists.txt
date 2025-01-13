# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


# Default board to blackpill
if (NOT DEFINED ${BOARD})
  set(BOARD blackpill_f411ce)
endif()

# Define board root to find our custom shield
set(BOARD_ROOT ${CMAKE_SOURCE_DIR})

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(imubar)

FILE(GLOB app_sources src/*.cpp)

target_sources(app PRIVATE ${app_sources})
