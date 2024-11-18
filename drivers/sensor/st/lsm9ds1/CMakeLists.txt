# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(lsm9ds1.c)
zephyr_library_sources_ifndef(CONFIG_LSM9DS1_TRIGGER_NONE lsm9ds1_trigger.c)
