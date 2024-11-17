---
title: IMU Bar
---

This is a Zephyr RTOS based project to read out and process data from a
number of IMU breakout board that have all been glued to a single piece
of wood. The goal is to fuse the data of all the sensors together and
compare the respective qualities of the signals. An STM32 BlackPill is
used to read out the data through two I2C ports:

    Address: Board - Chip: Type

    Port 0:

    19: ADAFRUIT 10DOF - LSM303DLHC: accel
    1c: SPARKFUN SEN13944 - LSM9DS1: magn
    1e: ADAFRUIT 10DOF - LSM303DLHC: magn
    1f: ADAFRUIT 9DOF - FXOS8700CQ: accel/magn
    21: ADAFRUIT 9DOF - FXAS21002C: gyro
    28: ADAFRUIT IMU - BNO055: accel/gyro/mag - fusion
    69: SPARKFUN SEN13762 - MPU9250: accel/gyro/magn
    6a: SPARKFUN SEN13944 - LSM9DS1: accel/gyro
    6b: ADAFRUIT 10DOF - L3GD20H: gyro
    77: ADAFRUIT 10DOF - BMP180: press/temp

    Port 1:

    19: SPARKFUN SEN14480 - H3LIS331DL: accel
    1e: NAMELESS 10DOF - HMC5883: magn
    40: NAMELESS 10DOF - BMA180: accel
    68: NAMELESS 10DOF - ITG3205: gyro
    69: PRIMORONI IMC20948 - ICM20948: accel/gyro/magn
    77: NAMELESS 10DOF - BMP085: pressure

# Flashing

## BlackPill

Boot the BlackPill with BOOT0 button down and use dfu-util -a 0 to
download or upload firmware
