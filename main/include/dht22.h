#ifndef DHT_H_
#define DHT_H_

#include "soc/gpio_num.h"
struct dht_return read_dht22(void);
struct dht_return {
  float humidity_percent;
  float temperature_c;
};
void set_dht22_gpio(gpio_num_t gpio);

#endif