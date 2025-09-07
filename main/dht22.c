
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include <stdint.h>
#include <sys/_intsup.h>

#include "include/dht22.h"

int DHT_GPIO = GPIO_NUM_4;

void set_dht22_gpio(gpio_num_t gpio) { DHT_GPIO = gpio; }

typedef int wait_us;

wait_us wait_for_level_change(int starting_level, int timeout_us) {
  int us = 0;

  // When the level changes from the starting_level exit and return the time it
  // took in us
  while (gpio_get_level(DHT_GPIO) == starting_level) {
    if (us > timeout_us) {
      return -1;
    }
    ++us;
    esp_rom_delay_us(1);
  }

  return us;
}

struct dht_return read_dht22(void) {
  ESP_LOGI("read_dht22", "Starting DHT22 read\n");

  // DHT sends back 40 bits (5 bytes)
  uint8_t data[5] = {0};
  uint8_t bit_index = 7;
  uint8_t byte_index = 0;

  // https: // github.com/gosouth/DHT22/blob/master/DHT22.pdf
  gpio_set_direction(DHT_GPIO, GPIO_MODE_OUTPUT);

  // MCU pull low for wait 3ms
  gpio_set_level(DHT_GPIO, 0);
  esp_rom_delay_us(3000);

  // MCU pull up for 25us
  gpio_set_level(DHT_GPIO, 1);
  esp_rom_delay_us(25);

  // Start reading from DHT
  gpio_set_direction(DHT_GPIO, GPIO_MODE_INPUT);

  // wait for DHT pull down then up for 80us each (add in 5us buffer)
  wait_for_level_change(0, 85);
  wait_for_level_change(1, 85);

  // DHT has started to send data

  int high_us = 0;
  for (int i = 0; i < 40; i++) {
    // Starts each bit by pulling low for 50us (6us buffer)
    wait_for_level_change(0, 56);
    high_us = wait_for_level_change(1, 75);

    // if pin was high for < 28us it was a 0 else it was a 1
    if (high_us > 40) {
      data[byte_index] |= (1 << bit_index);
    }
    // The array was initialized to 0 so no else block just move on for 0

    if (bit_index == 0) {
      bit_index = 7;
      ++byte_index;
    } else {
      bit_index--;
    }
  }

  // Debug: print raw data
  ESP_LOGI("debug", "Raw data: [%d, %d, %d, %d, %d]", data[0], data[1], data[2],
           data[3], data[4]);

  // DHT22 data format:
  // data[0]: humidity high byte
  // data[1]: humidity low byte
  // data[2]: temperature high byte (bit 7 = sign, bits 6-0 = integer part)
  // data[3]: temperature low byte (decimal part)
  // data[4]: checksum
  float temperature = 0.;
  temperature = data[2] & 0x7F;
  temperature *= 0x100; // >> 8
  temperature += data[3];
  temperature /= 10;

  if (data[2] & 0x80) // negative temp, brrr it's freezing
    temperature *= -1;

  ESP_LOGI("test", "temp: %.1f", temperature);

  struct dht_return res = {
      .humidity_percent = 50,
      .temperature_c = 25,
  };

  return res;
}