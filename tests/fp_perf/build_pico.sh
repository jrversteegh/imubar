#!/bin/sh

rm -rf build
# I can't get opencd to work at the moment. It keeps complaining: Error: unable to find a matching CMSIS-DAP device
#OOCD=`which openocd`
#west build -b rpi_pico . -- -DOPENOCD=$OOCD -DOPENOCD_DEFAULT_PATH=target -DRPI_PICO_DEBUG_ADAPTER=cmsis-dap
#west flash
west build -b rpi_pico
picotool load build/zephyr/zephyr.uf2
picotool reboot
