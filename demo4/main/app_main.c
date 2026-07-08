#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static QueueHandle_t number_queue;

static void sender_task(void *arg)
{
    (void)arg;

    int number = 0;

    while (1) {
        printf("sender_task: send %d\n", number);
        xQueueSend(number_queue, &number, portMAX_DELAY);
        number++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void receiver_task(void *arg)
{
    (void)arg;

    int received_number = 0;

    while (1) {
        if (xQueueReceive(number_queue, &received_number, portMAX_DELAY) == pdTRUE) {
            printf("receiver_task: received %d\n", received_number);
        }
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS queue demo\n");

    number_queue = xQueueCreate(5, sizeof(int));
    if (number_queue == NULL) {
        printf("failed to create queue\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 2048, NULL, 5, NULL);
    xTaskCreate(receiver_task, "receiver_task", 2048, NULL, 5, NULL);
}
