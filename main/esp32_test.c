#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/uart.h";
#include "portmacro.h"

void app_main(void) {
  char *taskName = pcTaskGetName(NULL);

  ESP_LOGI(taskName, "Hello, starting up \n");
  unsigned char c = 'a';
  while (1) {
    uart_get_byte(&c);
    ESP_LOGI(taskName, "Got char: %X, %c", c, c);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
