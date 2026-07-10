#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define LARGE_ARRAY_LEN 1024

// 大数组只传指针和长度，避免把 1024 个 int 全部拷贝进队列。
typedef struct {
    const int *data;
    size_t length;
} array_ptr_message_t;

static QueueHandle_t ptr_queue;
static int large_array[LARGE_ARRAY_LEN];

static void sender_task(void *arg)
{
    (void)arg;

    while (1) {
        // message 很小：只包含一个指针和一个长度。
        array_ptr_message_t message = {
            .data = large_array,
            .length = LARGE_ARRAY_LEN,
        };

        printf("sender_task: send large array pointer addr=%p length=%u\n",
               message.data,
               (unsigned int)message.length);
        xQueueSend(ptr_queue, &message, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void receiver_task(void *arg)
{
    (void)arg;

    array_ptr_message_t received_message;

    while (1) {
        if (xQueueReceive(ptr_queue, &received_message, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        printf("receiver_task: received large array pointer addr=%p length=%u first=%d last=%d\n",
               received_message.data,
               (unsigned int)received_message.length,
               received_message.data[0],
               received_message.data[received_message.length - 1]);
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS queue large array pointer demo\n");

    // large_array 是 static 全局数组，生命周期覆盖整个程序运行过程。
    for (size_t i = 0; i < LARGE_ARRAY_LEN; i++) {
        large_array[i] = (int)i;
    }

    // 队列项大小是指针消息，不是整个 large_array。
    ptr_queue = xQueueCreate(5, sizeof(array_ptr_message_t));
    if (ptr_queue == NULL) {
        printf("failed to create queue\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 2048, NULL, 5, NULL);
    xTaskCreate(receiver_task, "receiver_task", 2048, NULL, 5, NULL);
}
