#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

typedef struct {
    const char *data;
    size_t length;
} string_ptr_message_t;

static QueueHandle_t ptr_queue;

// static 全局字符串生命周期足够长，接收任务拿到指针后仍然能访问。
static const char large_text[] =
    "This is a large string demo. The queue only carries its pointer and length, "
    "so FreeRTOS does not copy the whole string into the queue item.";

static void sender_task(void *arg)
{
    (void)arg;

    while (1) {
        // 队列只拷贝 data 指针和 length，不拷贝 large_text 内容。
        string_ptr_message_t message = {
            .data = large_text,
            .length = strlen(large_text),
        };

        printf("sender_task: send large string pointer addr=%p length=%u\n",
               message.data,
               (unsigned int)message.length);
        xQueueSend(ptr_queue, &message, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void receiver_task(void *arg)
{
    (void)arg;

    string_ptr_message_t received_message;

    while (1) {
        if (xQueueReceive(ptr_queue, &received_message, portMAX_DELAY) == pdTRUE) {
            printf("receiver_task: received large string pointer addr=%p length=%u preview=%.*s\n",
                   received_message.data,
                   (unsigned int)received_message.length,
                   32,
                   received_message.data);
        }
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS queue large string pointer demo\n");

    // 适合传大块只读数据；注意不要传局部数组的地址。
    ptr_queue = xQueueCreate(5, sizeof(string_ptr_message_t));
    if (ptr_queue == NULL) {
        printf("failed to create queue\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 2048, NULL, 5, NULL);
    xTaskCreate(receiver_task, "receiver_task", 2048, NULL, 5, NULL);
}
