#!/bin/sh

rm -rf build
west build -b blackpill_f411ce --sysbuild .
west flash
