# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(lsm9ds1_ag.c)
zephyr_library_sources_ifndef(CONFIG_LSM9DS1_TRIGGER_NONE lsm9ds1_ag_trigger.c)
add_dependencies(${ZEPHYR_CURRENT_LIBRARY} offsets_h)
