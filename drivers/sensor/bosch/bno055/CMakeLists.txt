# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(bno055.c)
zephyr_library_sources_ifdef(CONFIG_BNO055_TRIGGER bno055_trigger.c)
add_dependencies(${ZEPHYR_CURRENT_LIBRARY} offsets_h)
