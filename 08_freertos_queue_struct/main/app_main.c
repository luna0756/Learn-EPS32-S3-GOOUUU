#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// 队列可以直接传结构体，适合少量字段组成的一条消息。
typedef struct {
    int id;
    float temperature;
    char name[16];
} sensor_data_t;

static QueueHandle_t data_queue;

static void sender_task(void *arg)
{
    (void)arg;

    int id = 0;

    while (1) {
        // data 是局部变量也没关系；xQueueSend 会把结构体内容拷贝进队列。
        sensor_data_t data = {
            .id = id,
            .temperature = 26.5f + id,
            .name = "esp32-s3",
        };

        printf("sender_task: send struct id=%d\n", data.id);
        xQueueSend(data_queue, &data, portMAX_DELAY);

        id++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void receiver_task(void *arg)
{
    (void)arg;

    sensor_data_t received_data;

    while (1) {
        if (xQueueReceive(data_queue, &received_data, portMAX_DELAY) == pdTRUE) {
            printf("receiver_task: received struct id=%d temperature=%.1f name=%s\n",
                   received_data.id,
                   received_data.temperature,
                   received_data.name);
        }
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS queue struct demo\n");

    // 队列里每一项的大小就是一个 sensor_data_t。
    data_queue = xQueueCreate(5, sizeof(sensor_data_t));
    if (data_queue == NULL) {
        printf("failed to create queue\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 2048, NULL, 5, NULL);
    xTaskCreate(receiver_task, "receiver_task", 2048, NULL, 5, NULL);
}
