
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// Datasheet:
// https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#iomuxgpio

#define BLINK_LED 2

// When you write to this register it will write to all the GPIOs, so if you
// write just one bit, it will write 0 to all the other bits
#define GPIO_OUT_REG 0x3FF44004

// When you write to this register it will write to the GPIO specified 0-31,
// means that it will only set the bit you want
#define GPIO_OUT_W1TS_REG 0x3FF44008

// When you write to this register it will clear the GPIO specified 0-31
#define GPIO_OUT_W1TC_REG 0x3FF4400C

// Enables the GPIO specified 0-31
#define GPIO_ENABLE_REG 0x3FF44020

typedef enum {
  GPIO_LEVEL_LOW = 0,
  GPIO_LEVEL_HIGH = 1,
} gpio_level;

void my_gpio_enable(gpio_num_t gpio_num) {
  // Get pointers to the registers, They are uint32 because they are 32 bits
  // wide (in datasheet)
  volatile uint32_t *gpio_enable_reg = (volatile uint32_t *)GPIO_ENABLE_REG;
  *gpio_enable_reg = (1 << gpio_num);
}

void my_gpio_set_level(gpio_num_t gpio_num, gpio_level level) {
  if (level == GPIO_LEVEL_LOW) {

    // get the reg
    volatile uint32_t *gpio_out_w1tc_reg =
        (volatile uint32_t *)GPIO_OUT_W1TC_REG;

    // set the bit you want to clear
    *gpio_out_w1tc_reg = (1 << gpio_num);

  } else {

    volatile uint32_t *gpio_out_w1ts_reg =
        (volatile uint32_t *)GPIO_OUT_W1TS_REG;

    // set the bit you want to set
    *gpio_out_w1ts_reg = (1 << gpio_num);
  }
}