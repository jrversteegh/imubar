#ifndef IMUBAR_INTERFACE_DISPLAY_H__
#define IMUBAR_INTERFACE_DISPLAY_H__

#include <zephyr/drivers/sensor.h>

extern void display_set_backlight(uint8_t brightness);
extern void display_update_backlight();
extern void display_update();
extern void display_show_sensor_value(sensor_value value);
extern void display_show_message(char const* msg);
extern void display_init();

extern void display_test();

#endif
