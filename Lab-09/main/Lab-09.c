#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED1_GPIO GPIO_NUM_2
#define LED2_GPIO GPIO_NUM_4
#define LED3_GPIO GPIO_NUM_5

static const char *TAG = "LED3";
static const gpio_num_t LEDS[] = { LED1_GPIO, LED2_GPIO, LED3_GPIO };
#define LED_COUNT (sizeof(LEDS) / sizeof(LEDS[0]))

static void led_init(void)
{
    for (size_t i = 0; i < LED_COUNT; i++) {
        gpio_reset_pin(LEDS[i]);
    }

    uint64_t mask = (1ULL << LED1_GPIO) | (1ULL << LED2_GPIO) | (1ULL << LED3_GPIO);

    gpio_config_t cfg = {
        .pin_bit_mask = mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));

    for (size_t i = 0; i < LED_COUNT; i++) {
        gpio_set_level(LEDS[i], 0);
    }

    ESP_LOGI(TAG, "Initialized %u LEDs (GPIO 2,4,5)", (unsigned)LED_COUNT);
}

static void led_blink_task(void *arg)
{
    const TickType_t delay = pdMS_TO_TICKS(500);
    int level = 0;

    while (1) {
        level = !level;
        for (size_t i = 0; i < LED_COUNT; i++) {
            gpio_set_level(LEDS[i], level);
        }
        vTaskDelay(delay);
    }
}

void app_main(void)
{
    led_init();
    xTaskCreate(led_blink_task, "blink3", 2048, NULL, 5, NULL);
}
