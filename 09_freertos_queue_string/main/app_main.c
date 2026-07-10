#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define TEXT_MAX_LEN 32

// C 字符串本质是 char 数组；这个示例把短字符串数组整体拷贝进队列。
typedef struct {
    char text[TEXT_MAX_LEN];
} text_message_t;

static QueueHandle_t text_queue;

static void sender_task(void *arg)
{
    (void)arg;

    int count = 0;

    while (1) {
        text_message_t message;

        // snprintf 会自动补字符串结束符 '\0'，并限制最大写入长度。
        snprintf(message.text, sizeof(message.text), "hello queue %d", count);

        printf("sender_task: send string %s\n", message.text);
        xQueueSend(text_queue, &message, portMAX_DELAY);

        count++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void receiver_task(void *arg)
{
    (void)arg;

    text_message_t received_message;

    while (1) {
        if (xQueueReceive(text_queue, &received_message, portMAX_DELAY) == pdTRUE) {
            printf("receiver_task: received string = %s\n", received_message.text);
        }
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS queue string demo\n");

    // 每个队列项固定为 TEXT_MAX_LEN 字节的字符串消息。
    text_queue = xQueueCreate(5, sizeof(text_message_t));
    if (text_queue == NULL) {
        printf("failed to create queue\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 2048, NULL, 5, NULL);
    xTaskCreate(receiver_task, "receiver_task", 2048, NULL, 5, NULL);
}
