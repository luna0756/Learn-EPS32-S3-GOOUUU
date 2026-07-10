#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS scheduler note demo\n");
    printf("ESP-IDF already starts the FreeRTOS scheduler before app_main runs.\n");

    /*
     * 裸 FreeRTOS 工程通常会在创建任务后调用：
     *
     *     vTaskStartScheduler();
     *
     * ESP-IDF 不是这样写。这里不能再次调用 vTaskStartScheduler()，
     * 否则会破坏 ESP-IDF 已经启动好的系统调度流程。
     */

    while (1) {
        printf("app_main task is running under the scheduler\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
