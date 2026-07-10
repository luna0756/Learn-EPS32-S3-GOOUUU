#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "hello GOOUUU ESP32-S3-CAM");

    // app_main 本身也是一个 FreeRTOS 任务；这里用死循环持续打印日志。
    while (1) {
        ESP_LOGI(TAG, "app is running...");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 暂停 1000 毫秒，也就是 1 秒。
    }
}
