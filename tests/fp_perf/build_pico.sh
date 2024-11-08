#!/bin/sh

rm -rf build
OOCD=`which openocd`
west build -b rpi_pico . -- -DOPENOCD=$OOCD -DOPENOCD_DEFAULT_PATH=target -DRPI_PICO_DEBUG_ADAPTER=cmsis-dap
west flash
