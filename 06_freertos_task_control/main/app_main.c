#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t periodic_task_handle;

static void periodic_task(void *arg)
{
    (void)arg;

    TickType_t last_wake_time = xTaskGetTickCount();
    int count = 0;

    while (1) {
        printf("periodic_task: count=%d, fixed 500ms period\n", count++);

        // vTaskDelayUntil 适合固定周期任务，比简单 vTaskDelay 更不容易累积误差。
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(500));
    }
}

static void controller_task(void *arg)
{
    (void)arg;

    TickType_t last_wake_time = xTaskGetTickCount();

    vTaskDelayUntil(&last_wake_time,pdMS_TO_TICKS(3000));
    printf("controller_task: suspend periodic_task\n");
    vTaskSuspend(periodic_task_handle);

    vTaskDelayUntil(&last_wake_time,pdMS_TO_TICKS(3000));
    printf("controller_task: resume periodic_task\n");
    vTaskResume(periodic_task_handle);

    vTaskDelayUntil(&last_wake_time,pdMS_TO_TICKS(3000));
    printf("controller_task: delete periodic_task\n");
    vTaskDelete(periodic_task_handle);
    periodic_task_handle = NULL;

    printf("controller_task: delete itself\n");
    vTaskDelete(NULL);
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS task control demo\n");

    xTaskCreate(periodic_task, "periodic_task", 2048, NULL, 5, &periodic_task_handle);
    xTaskCreate(controller_task, "controller_task", 2048, NULL, 6, NULL);
}
