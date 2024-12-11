#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>

#include "errors.h"
#include "display.h"

LOG_MODULE_DECLARE(imubar_interface);

#define BACKLIGHT0_NODE DT_ALIAS(backlight0)

static const struct pwm_dt_spec backlight = PWM_DT_SPEC_GET(BACKLIGHT0_NODE);
static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

void set_backlight(uint8_t brightness) {
  pwm_set_dt(&backlight, 255000, 1000 * brightness);
}


void test_display() {
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_montserrat_16);
  lv_style_set_text_color(&style, lv_color_hex(0xFFFFFF));
  static lv_obj_t * my_rect = lv_obj_create(lv_scr_act());
  lv_obj_set_size(my_rect, 160, 128);
  lv_obj_set_pos(my_rect, 0, 0);
  lv_obj_set_style_bg_color(my_rect , lv_color_hex(0x00FFFF), 0);
  static lv_obj_t * my_rect2 = lv_obj_create(lv_scr_act());
  lv_obj_set_size(my_rect2, 144, 42);
  lv_obj_align(my_rect2, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_color(my_rect2 , lv_color_hex(0xFF0000), 0);
  static lv_obj_t *hello_world_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(hello_world_label, &style, 0);
  //lv_label_set_text(hello_world_label, LV_SYMBOL_BATTERY_FULL);
  lv_label_set_text(hello_world_label, "12345.67890");
  lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);
}

void initialize_display() {
  if (!device_is_ready(display_dev)) {
    error(2, "Display device not ready.");
  }

  if (!pwm_is_ready_dt(&backlight)) {
    error(2, "Backlight control not ready.");
  }

  LOG_INF("Successful display init");
}
