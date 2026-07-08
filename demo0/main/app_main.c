#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "hello GOOUUU ESP32-S3-CAM");

    while (1) {
        ESP_LOGI(TAG, "app is running...");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 让当前任务暂停 1000 毫秒，也就是 1 秒。
    }
}
