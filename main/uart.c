#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "hal/gpio_types.h"

// Bit banging uart cuz why not

#define SECONDS_PER_MICROSECOND 1'000'000

int TX_PIN = 4;
int baud_rate = 9600;

void uart_set_tx_pin(int gpio_num) { TX_PIN = gpio_num; }
void uart_set_baud_rate(int rate) { baud_rate = rate; }

void uart_send_byte(char c) {

  float dt_us = (1.0 / baud_rate) * SECONDS_PER_MICROSECOND;

  // Hold high for idle on TX pin
  gpio_set_direction(TX_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(TX_PIN, 1);
  esp_rom_delay_us(dt_us);

  // low for start bit
  gpio_set_level(TX_PIN, 0);
  esp_rom_delay_us(dt_us);

  unsigned char mask = 1;

  // Start writing data
  for (int i = 0; i < 8; i++) {
    if (mask & c) {
      gpio_set_level(TX_PIN, 1);
    } else {
      gpio_set_level(TX_PIN, 0);
    }
    mask <<= 1;

    esp_rom_delay_us(dt_us);
  }

  // 1 stop bit
  gpio_set_level(TX_PIN, 1);
  esp_rom_delay_us(dt_us);
}

void uart_send_str(char *str) {
  while (*str) {
    uart_send_byte(*str++);
  }
}

int RX_PIN = 4;

void uart_get_byte(char *byte) {

  float dt_us = (1.0 / baud_rate) * SECONDS_PER_MICROSECOND;

  gpio_set_direction(RX_PIN, GPIO_MODE_INPUT);

  while (gpio_get_level(RX_PIN) == 1)
    // pin set to low, this is the start bit
    esp_rom_delay_us(dt_us);

  // start reading data
  for (int i = 0; i < 8; i++) {
    if (gpio_get_level(RX_PIN) == 1) {
      *byte |= 1 << i;
    } else {
      *byte &= ~(1 << i);
    }
    esp_rom_delay_us(dt_us);
  }
  esp_rom_delay_us(dt_us);
}
