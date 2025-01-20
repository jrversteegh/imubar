#ifndef IMUBAR_INTERFACE_DISPLAY_H__
#define IMUBAR_INTERFACE_DISPLAY_H__

#include <zephyr/drivers/sensor.h>

namespace imubar {
namespace display {

extern void set_backlight(uint8_t brightness);
extern void update_backlight();
extern void update();
extern void show_sensor_value(sensor_value value);
extern void show_message(char const* msg);
extern void initialize();

extern void display_test();

}  // namespace display
}  // namespace imubar

#endif
