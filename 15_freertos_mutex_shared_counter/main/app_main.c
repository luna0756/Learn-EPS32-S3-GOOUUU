#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static SemaphoreHandle_t counter_mutex;
static int shared_counter;

static void counter_task(void *arg)
{
    const char *name = (const char *)arg;

    while (1) {
        if (xSemaphoreTake(counter_mutex, portMAX_DELAY) == pdTRUE) {
            int next_value = shared_counter + 1;
            shared_counter = next_value;
            printf("%s: shared_counter=%d\n", name, shared_counter);

            // 互斥锁拿到后一定要释放，否则其他任务会一直等。
            xSemaphoreGive(counter_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(700));
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS mutex demo\n");

    counter_mutex = xSemaphoreCreateMutex();
    if (counter_mutex == NULL) {
        printf("failed to create mutex\n");
        return;
    }

    xTaskCreate(counter_task, "counter_a", 2048, "counter_a", 5, NULL);
    xTaskCreate(counter_task, "counter_b", 2048, "counter_b", 5, NULL);
}
