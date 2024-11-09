#!/bin/sh

rm -rf build
west build -b bbc_microbit_v2 --sysbuild .
west flash
