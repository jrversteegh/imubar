# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(lsm9ds1_magn.c)
add_dependencies(${ZEPHYR_CURRENT_LIBRARY} offsets_h)
