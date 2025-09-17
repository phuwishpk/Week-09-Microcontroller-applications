#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// === ปรับให้ตรงบอร์ดคุณ ===
#define LED1_GPIO GPIO_NUM_2
#define LED2_GPIO GPIO_NUM_4
#define LED3_GPIO GPIO_NUM_5

// ถ้า LED เป็น active-low ให้ตั้ง ON=0, OFF=1
#define ON_LEVEL  1
#define OFF_LEVEL 0

static const char *TAG = "LED_PATTERN";

// อาร์เรย์ขาเรียงจากซ้ายไปขวา (ปรับตามการวางจริงของคุณได้)
static const gpio_num_t LEDS[] = { LED1_GPIO, LED2_GPIO, LED3_GPIO };
#define LED_COUNT (sizeof(LEDS)/sizeof(LEDS[0]))

static void led_init(void)
{
    // reset pin ก่อน ป้องกันสถานะค้าง
    for (size_t i = 0; i < LED_COUNT; i++) {
        gpio_reset_pin(LEDS[i]);
    }

    // รวม bit mask เพื่อตั้งค่า OUTPUT ทีเดียว
    uint64_t mask = 0;
    for (size_t i = 0; i < LED_COUNT; i++) {
        mask |= (1ULL << LEDS[i]);
    }

    gpio_config_t cfg = {
        .pin_bit_mask = mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));

    // ดับทุกดวงก่อนเริ่ม
    for (size_t i = 0; i < LED_COUNT; i++) {
        gpio_set_level(LEDS[i], OFF_LEVEL);
    }

    ESP_LOGI(TAG, "Initialized %u LEDs on GPIOs: %d, %d, %d",
             (unsigned)LED_COUNT, LEDS[0], LEDS[1], LEDS[2]);
}

// ปิดทุกดวง ยกเว้น index ที่กำหนดให้ติด พร้อม log ว่าขาไหน “ถูกดับ”
static void set_only(int on_index)
{
    for (int i = 0; i < (int)LED_COUNT; i++) {
        if (i == on_index) {
            gpio_set_level(LEDS[i], ON_LEVEL);
        } else {
            gpio_set_level(LEDS[i], OFF_LEVEL);
            // พิมพ์ระบุว่าขานี้ถูกดับ
            // ใช้ทั้ง printf (ไป UART โดยตรง) และ ESP_LOGI (ดูใน monitor ได้เช่นกัน)
            printf("OFF GPIO%d\n", (int)LEDS[i]);
            ESP_LOGI(TAG, "OFF GPIO%d", (int)LEDS[i]);
        }
    }
}

// ไหลจากซ้ายไปขวา (index 0 -> LED_COUNT-1)
static void sweep_left_to_right(TickType_t delay_ticks)
{
    for (int i = 0; i < (int)LED_COUNT; i++) {
        set_only(i);
        vTaskDelay(delay_ticks);
    }
}

// ไหลจากขวาไปซ้าย (index LED_COUNT-1 -> 0)
static void sweep_right_to_left(TickType_t delay_ticks)
{
    for (int i = (int)LED_COUNT - 1; i >= 0; i--) {
        set_only(i);
        vTaskDelay(delay_ticks);
    }
}

static void led_pattern_task(void *arg)
{
    const TickType_t step_delay = pdMS_TO_TICKS(250); // หน่วงต่อก้าว (ปรับได้)

    while (1) {
        // รอบที่ 1: ซ้าย -> ขวา
        sweep_left_to_right(step_delay);

        // รอบที่ 2: ขวา -> ซ้าย
        sweep_right_to_left(step_delay);
    }
}

void app_main(void)
{
    led_init();
    xTaskCreate(led_pattern_task, "led_pattern", 2048, NULL, 5, NULL);
}
