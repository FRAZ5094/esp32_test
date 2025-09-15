
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"

float speed_of_sound_cm_us = 0.0343;

float TRIG_PIN = GPIO_NUM_26;
float ECHO_PIN = GPIO_NUM_25;

float get_distance() {
  gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
  gpio_set_level(TRIG_PIN, 0);
  esp_rom_delay_us(2);
  gpio_set_level(TRIG_PIN, 1);
  esp_rom_delay_us(10);
  gpio_set_level(TRIG_PIN, 0);
  // Wait for ECHO_PIN to go high
  while (gpio_get_level(ECHO_PIN) == 0)
    ;
  int64_t start_time = esp_timer_get_time();
  // Wait for ECHO_PIN to go low
  while (gpio_get_level(ECHO_PIN) == 1)
    ;
  int64_t end_time = esp_timer_get_time();

  int64_t us = end_time - start_time;

  if (us >= 36000) {
    // no object detected
    return 0;
  }

  float cm = (us * speed_of_sound_cm_us) / 2;

  return cm;
}
