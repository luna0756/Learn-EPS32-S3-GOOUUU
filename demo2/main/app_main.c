
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEMO_GPIO GPIO_NUM_2

static const char *TAG = "demo2_gpio";

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM GPIO output demo\n");

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << DEMO_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    int level = 0;

    while (1) {
        ESP_ERROR_CHECK(gpio_set_level(DEMO_GPIO, level));
        ESP_LOGI(TAG, "GPIO%d = %d", DEMO_GPIO, level);
        level = !level;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
