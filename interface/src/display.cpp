#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>

#include "errors.h"
#include "display.h"

LV_FONT_DECLARE(font_8);
LV_FONT_DECLARE(font_12);
LV_FONT_DECLARE(font_14);
LV_FONT_DECLARE(font_14_bold);
LV_FONT_DECLARE(font_18);
LV_FONT_DECLARE(font_18_bold);
LV_FONT_DECLARE(symbols_14);
LV_FONT_DECLARE(symbols_18);
LV_FONT_DECLARE(symbols_22);

LOG_MODULE_DECLARE(imubar_interface);

#define BACKLIGHT0_NODE DT_ALIAS(backlight0)

static const struct pwm_dt_spec backlight = PWM_DT_SPEC_GET(BACKLIGHT0_NODE);
static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

void set_backlight(uint8_t brightness) {
  pwm_set_dt(&backlight, 255000, 1000 * brightness);
}


void test_display() {
  static lv_style_t style1;
  static lv_style_t style2;
  static lv_style_t style3;
  static lv_style_t style4;
  static lv_style_t style5;
  static lv_style_t style6;

  lv_style_init(&style1);
  lv_style_set_text_font(&style1, &font_8);
  lv_style_set_text_color(&style1, lv_color_hex(0xFFFFFF));

  lv_style_init(&style2);
  lv_style_set_text_font(&style2, &font_12);
  lv_style_set_text_color(&style2, lv_color_hex(0xFFFFFF));

  lv_style_init(&style3);
  lv_style_set_text_font(&style3, &font_14);
  lv_style_set_text_color(&style3, lv_color_hex(0xFFFFFF));

  lv_style_init(&style4);
  lv_style_set_text_font(&style4, &symbols_14);
  lv_style_set_text_color(&style4, lv_color_hex(0xFFFFFF));

  lv_style_init(&style5);
  lv_style_set_text_font(&style5, &symbols_18);
  lv_style_set_text_color(&style5, lv_color_hex(0xFFFFFF));

  lv_style_init(&style6);
  lv_style_set_text_font(&style6, &symbols_22);
  lv_style_set_text_color(&style6, lv_color_hex(0xFFFFFF));


  static lv_obj_t * rect1 = lv_obj_create(lv_scr_act());
  lv_obj_set_size(rect1, 160, 128);
  lv_obj_set_pos(rect1, 0, 0);
  lv_obj_set_style_bg_color(rect1 , lv_color_hex(0x00FFFF), 0);

  static lv_obj_t * rect2 = lv_obj_create(lv_scr_act());
  lv_obj_set_size(rect2, 144, 112);
  lv_obj_set_style_pad_all(rect2, 5, 0);
  lv_obj_align(rect2, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_color(rect2 , lv_color_hex(0xFF0000), 0);

  static lv_obj_t *label1 = lv_label_create(rect2);
  lv_obj_add_style(label1, &style1, 0);
  lv_label_set_text(label1, "12345.67890");
  lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);

  static lv_obj_t *label2 = lv_label_create(rect2);
  lv_obj_add_style(label2, &style2, 0);
  lv_label_set_text(label2, "12345.67890");
  lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, -4);

  static lv_obj_t *label3 = lv_label_create(rect2);
  lv_obj_add_style(label3, &style3, 0);
  lv_label_set_text(label3, "12345.67890");
  lv_obj_align_to(label3, label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  static lv_obj_t *label4 = lv_label_create(rect2);
  lv_obj_add_style(label4, &style4, 0);
  lv_label_set_text(label4, LV_SYMBOL_BATTERY_2);
  lv_obj_align_to(label4, label3, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  static lv_obj_t *label5 = lv_label_create(rect2);
  lv_obj_add_style(label5, &style5, 0);
  lv_label_set_text(label5, LV_SYMBOL_BATTERY_2);
  lv_obj_align_to(label5, label4, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  static lv_obj_t *label6 = lv_label_create(rect2);
  lv_obj_add_style(label6, &style6, 0);
  lv_label_set_text(label6, LV_SYMBOL_BATTERY_2);
  lv_obj_align_to(label6, label5, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
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
