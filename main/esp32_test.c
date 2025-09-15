#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/uart.h";
#include "portmacro.h"

void app_main(void) {
  char *taskName = pcTaskGetName(NULL);

  ESP_LOGI(taskName, "Hello, starting up \n");

  while (1) {
    // uart_send_str("Sending data using uart\r\n");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
