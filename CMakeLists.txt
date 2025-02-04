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

# For some reason west doesn't want to link in our driver sources...
#include(${ZEPHYR_BASE}/cmake/modules/extensions.cmake)
#add_subdirectory(drivers)
#target_sources(app PRIVATE ${app_sources})

# ... so add them explicitly
set(driver_sources 
  drivers/sensor/bosch/bno055/bno055.c
  drivers/sensor/st/lsm9ds1_ag/lsm9ds1_ag.c
  drivers/sensor/st/lsm9ds1_magn/lsm9ds1_magn.c
  drivers/sensor/st/l3gd20h/l3gd20h.c
  drivers/sensor/bosch/bmp085/bmp085.c
  drivers/sensor/bosch/bma180/bma180.c
  drivers/sensor/tdk/itg3205/itg3205.c
  drivers/sensor/tdk/icm20948/icm20948.c
  drivers/sensor/st/h3lis331dl/h3lis331dl.c
)
target_sources(app PRIVATE ${app_sources} ${driver_sources})

include(ExternalProject)

set(ATTITUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/depends/attitude)
set(ATTITUDE_INCLUDE_DIR ${ATTITUDE_DIR}/include)
set(ATTITUDE_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/attitude)
set(ATTITUDE_LIB ${ATTITUDE_BUILD_DIR}/libattitude.a)

zephyr_get_include_directories_for_lang_as_string(       CPP CXX_INCLUDES)
zephyr_get_system_include_directories_for_lang_as_string(CPP CXX_SYSTEM_INCLUDES)
zephyr_get_compile_definitions_for_lang_as_string(       CPP CXX_DEFINITIONS)
zephyr_get_compile_options_for_lang_as_string(           CPP CXX_OPTIONS)


message("Compiler: ${CMAKE_CXX_COMPILER}")

ExternalProject_Add(
  attitude_src
  SOURCE_DIR ${ATTITUDE_DIR}
  PREFIX ${ATTITUDE_BUILD_DIR}
  BINARY_DIR ${ATTITUDE_BUILD_DIR}
  BUILD_BYPRODUCTS ${ATTITUDE_LIB}
  CMAKE_ARGS 
    -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY
    -DCMAKE_SYSTEM_NAME=Generic
    -DCMAKE_CROSSCOMPILING=1
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_COMPILER_WORKS=1
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CXX_OPTIONS}
  INSTALL_COMMAND ""
)

add_library(attitude_lib STATIC IMPORTED GLOBAL)
add_dependencies(attitude_lib attitude_src)


add_dependencies(app attitude_lib)
set_target_properties(attitude_lib PROPERTIES IMPORTED_LOCATION             ${ATTITUDE_LIB}) 
set_target_properties(attitude_lib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ATTITUDE_INCLUDE_DIR})

target_link_libraries(app PUBLIC attitude_lib)
target_include_directories(app PUBLIC ${ATTITUDE_INCLUDE_DIR})
