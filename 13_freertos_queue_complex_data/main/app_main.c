#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define SMALL_TEXT_MAX_LEN 32
#define SMALL_ARRAY_LEN 5
#define LARGE_ARRAY_LEN 1024

// 示例用传感器数据模拟“结构体消息”。
typedef struct {
    int id;
    float temperature;
    char name[16];
} sensor_data_t;

// type 字段告诉接收任务：当前 payload 里放的到底是哪一种数据。
typedef enum {
    MESSAGE_TYPE_STRUCT = 0,
    MESSAGE_TYPE_STRING,
    MESSAGE_TYPE_ARRAY,
    MESSAGE_TYPE_LARGE_STRING_PTR,
    MESSAGE_TYPE_LARGE_ARRAY_PTR,
} message_type_t;

typedef struct {
    message_type_t type;

    // union 让多种消息共用同一段内存，队列项不会随着类型数量无限变大。
    union {
        sensor_data_t sensor;

        char text[SMALL_TEXT_MAX_LEN];

        struct {
            int values[SMALL_ARRAY_LEN];
            size_t length;
        } array;

        struct {
            const char *data;
            size_t length;
        } string_ptr;

        struct {
            const int *data;
            size_t length;
        } array_ptr;
    } payload;
} queue_message_t;

static QueueHandle_t message_queue;

/*
 * 大数据只传指针时，被指向的数据生命周期必须比接收任务更长。
 * 这里使用 static 全局数据，保证接收任务拿到指针时数据仍然有效。
 */
static const char large_text[] =
    "This is a large string demo. The queue only carries its pointer and length, "
    "so FreeRTOS does not copy the whole string into the queue item.";

static int large_array[LARGE_ARRAY_LEN];

static void send_struct_message(void)
{
    // 小结构体可以直接放进队列，FreeRTOS 会拷贝整个 queue_message_t。
    queue_message_t message = {
        .type = MESSAGE_TYPE_STRUCT,
        .payload.sensor = {
            .id = 1,
            .temperature = 26.5f,
            .name = "esp32-s3",
        },
    };

    printf("sender_task: send struct\n");
    xQueueSend(message_queue, &message, portMAX_DELAY);
}

static void send_string_message(void)
{
    queue_message_t message = {
        .type = MESSAGE_TYPE_STRING,
    };

    snprintf(message.payload.text, sizeof(message.payload.text), "hello queue");

    printf("sender_task: send string\n");
    xQueueSend(message_queue, &message, portMAX_DELAY);
}

static void send_array_message(int round)
{
    queue_message_t message = {
        .type = MESSAGE_TYPE_ARRAY,
        .payload.array.length = SMALL_ARRAY_LEN,
    };

    for (size_t i = 0; i < SMALL_ARRAY_LEN; i++) {
        message.payload.array.values[i] = round * 10 + (int)i;
    }

    printf("sender_task: send small array\n");
    xQueueSend(message_queue, &message, portMAX_DELAY);
}

static void send_large_string_ptr_message(void)
{
    // 大字符串不直接拷贝进队列，只发送地址和长度。
    queue_message_t message = {
        .type = MESSAGE_TYPE_LARGE_STRING_PTR,
        .payload.string_ptr = {
            .data = large_text,
            .length = strlen(large_text),
        },
    };

    printf("sender_task: send large string pointer\n");
    xQueueSend(message_queue, &message, portMAX_DELAY);
}

static void send_large_array_ptr_message(void)
{
    // 大数组同理：队列里只放指针和长度，接收方通过指针读取原数组。
    queue_message_t message = {
        .type = MESSAGE_TYPE_LARGE_ARRAY_PTR,
        .payload.array_ptr = {
            .data = large_array,
            .length = LARGE_ARRAY_LEN,
        },
    };

    printf("sender_task: send large array pointer\n");
    xQueueSend(message_queue, &message, portMAX_DELAY);
}

static void sender_task(void *arg)
{
    (void)arg;

    int round = 0;

    while (1) {
        send_struct_message();
        vTaskDelay(pdMS_TO_TICKS(500));

        send_string_message();
        vTaskDelay(pdMS_TO_TICKS(500));

        send_array_message(round);
        vTaskDelay(pdMS_TO_TICKS(500));

        send_large_string_ptr_message();
        vTaskDelay(pdMS_TO_TICKS(500));

        send_large_array_ptr_message();
        vTaskDelay(pdMS_TO_TICKS(2000));

        round++;
    }
}

static void print_small_array(const int *values, size_t length)
{
    printf("receiver_task: received array = [");
    for (size_t i = 0; i < length; i++) {
        printf("%d%s", values[i], (i + 1 == length) ? "" : ", ");
    }
    printf("]\n");
}

static void receiver_task(void *arg)
{
    (void)arg;

    queue_message_t message;

    while (1) {
        if (xQueueReceive(message_queue, &message, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        switch (message.type) {
        case MESSAGE_TYPE_STRUCT:
            // 先判断 type，再读取对应的 union 成员。
            printf("receiver_task: received struct id=%d temperature=%.1f name=%s\n",
                   message.payload.sensor.id,
                   message.payload.sensor.temperature,
                   message.payload.sensor.name);
            break;

        case MESSAGE_TYPE_STRING:
            printf("receiver_task: received string = %s\n", message.payload.text);
            break;

        case MESSAGE_TYPE_ARRAY:
            print_small_array(message.payload.array.values, message.payload.array.length);
            break;

        case MESSAGE_TYPE_LARGE_STRING_PTR:
            printf("receiver_task: received large string pointer addr=%p length=%u preview=%.*s\n",
                   message.payload.string_ptr.data,
                   (unsigned int)message.payload.string_ptr.length,
                   32,
                   message.payload.string_ptr.data);
            break;

        case MESSAGE_TYPE_LARGE_ARRAY_PTR:
            printf("receiver_task: received large array pointer addr=%p length=%u first=%d last=%d\n",
                   message.payload.array_ptr.data,
                   (unsigned int)message.payload.array_ptr.length,
                   message.payload.array_ptr.data[0],
                   message.payload.array_ptr.data[message.payload.array_ptr.length - 1]);
            break;

        default:
            printf("receiver_task: unknown message type %d\n", message.type);
            break;
        }
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS queue complex data demo\n");

    // 初始化大数组。因为它是 static 全局变量，指针传给接收任务后仍然有效。
    for (size_t i = 0; i < LARGE_ARRAY_LEN; i++) {
        large_array[i] = (int)i;
    }

    // 队列项大小是 queue_message_t；发送时会拷贝整个消息头和 payload。
    message_queue = xQueueCreate(5, sizeof(queue_message_t));
    if (message_queue == NULL) {
        printf("failed to create queue\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 3072, NULL, 5, NULL);
    xTaskCreate(receiver_task, "receiver_task", 3072, NULL, 5, NULL);
}
