#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "HELLO";

void app_main(void)
{
    while (1) {
        // ส่งออกผ่าน UART (115200 ตามดีฟอลต์)
        printf("Hello, world from Docker!\n");
        ESP_LOGI(TAG, "Tick: %lu", (unsigned long)xTaskGetTickCount());

        vTaskDelay(pdMS_TO_TICKS(1000)); // หน่วง 1 วินาที
    }
}
