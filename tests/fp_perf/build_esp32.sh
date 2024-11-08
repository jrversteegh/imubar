#!/bin/sh

rm -rf build
west blobs fetch hal_espressif
west build -b esp_wrover_kit --sysbuild .
west flash
west espressif monitor
