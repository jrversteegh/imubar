# Copyright (c) 2024 Analog Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

# zephyr-keep-sorted-start
add_subdirectory_ifdef(CONFIG_BMA180 bma180)
add_subdirectory_ifdef(CONFIG_BMP085 bmp085)
add_subdirectory_ifdef(CONFIG_BNO055 bno055)
# zephyr-keep-sorted-stop
