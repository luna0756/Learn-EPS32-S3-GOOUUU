#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define ARRAY_LEN 5

// 小数组可以直接放进结构体，再整体拷贝进队列。
typedef struct {
    int values[ARRAY_LEN];
    size_t length;
} array_message_t;

static QueueHandle_t array_queue;

static void sender_task(void *arg)
{
    (void)arg;

    int round = 0;

    while (1) {
        array_message_t message = {
            .length = ARRAY_LEN,
        };

        // 生成一组容易观察的测试数据。
        for (size_t i = 0; i < ARRAY_LEN; i++) {
            message.values[i] = round * 10 + (int)i;
        }

        printf("sender_task: send array round=%d\n", round);
        xQueueSend(array_queue, &message, portMAX_DELAY);

        round++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void receiver_task(void *arg)
{
    (void)arg;

    array_message_t received_message;

    while (1) {
        if (xQueueReceive(array_queue, &received_message, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        printf("receiver_task: received array = [");
        for (size_t i = 0; i < received_message.length; i++) {
            printf("%d%s", received_message.values[i], (i + 1 == received_message.length) ? "" : ", ");
        }
        printf("]\n");
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS queue array demo\n");

    // 队列项包含整个数组，所以数组越大，队列拷贝成本越高。
    array_queue = xQueueCreate(5, sizeof(array_message_t));
    if (array_queue == NULL) {
        printf("failed to create queue\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 2048, NULL, 5, NULL);
    xTaskCreate(receiver_task, "receiver_task", 2048, NULL, 5, NULL);
}
