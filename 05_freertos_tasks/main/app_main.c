#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 任务函数必须是 void (*)(void *) 这种形式，参数不用时显式忽略。
static void task_1s(void *arg)
{
    (void)arg;

    while (1) {
        printf("task_1s: every 1000 ms\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void task_500ms(void *arg)
{
    (void)arg;

    while (1) {
        printf("task_500ms: every 500 ms\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS task demo\n");

    // 两个任务互不阻塞，各自按照自己的延时周期运行。
    xTaskCreate(task_1s, "task_1s", 2048, NULL, 5, NULL);
    xTaskCreate(task_500ms, "task_500ms", 2048, NULL, 5, NULL);
}
