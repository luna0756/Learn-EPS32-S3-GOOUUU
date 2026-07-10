#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEMO_GPIO GPIO_NUM_2
#define BLINK_INTERVAL_MS 1000

static const char *TAG = "gpio_output";

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM GPIO output demo\n");

    // 把 GPIO2 配置成普通推挽输出，不使用上下拉和中断。
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
        // 每次循环写一个电平，然后取反，形成 0/1 交替输出。
        ESP_ERROR_CHECK(gpio_set_level(DEMO_GPIO, level));
        ESP_LOGI(TAG, "GPIO%d = %d", DEMO_GPIO, level);

        level = !level;

        vTaskDelay(pdMS_TO_TICKS(BLINK_INTERVAL_MS));
    }
}
