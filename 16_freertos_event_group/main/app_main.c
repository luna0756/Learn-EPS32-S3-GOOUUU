#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#define WIFI_READY_BIT BIT0
#define MQTT_READY_BIT BIT1
#define ALL_READY_BITS (WIFI_READY_BIT | MQTT_READY_BIT)

static EventGroupHandle_t app_event_group;

static void producer_task(void *arg)
{
    (void)arg;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("producer_task: set WIFI_READY_BIT\n");
        xEventGroupSetBits(app_event_group, WIFI_READY_BIT);

        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("producer_task: set MQTT_READY_BIT\n");
        xEventGroupSetBits(app_event_group, MQTT_READY_BIT);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void waiter_task(void *arg)
{
    (void)arg;

    while (1) {
        printf("waiter_task: wait WIFI and MQTT ready\n");

        EventBits_t bits = xEventGroupWaitBits(
            app_event_group,
            ALL_READY_BITS,
            pdTRUE,  // 返回后自动清除这些 bit，方便下一轮演示。
            pdTRUE,  // 必须两个 bit 都到齐才返回。
            portMAX_DELAY);

        printf("waiter_task: all ready, bits=0x%02x\n", (unsigned int)bits);
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS event group demo\n");

    app_event_group = xEventGroupCreate();
    if (app_event_group == NULL) {
        printf("failed to create event group\n");
        return;
    }

    xTaskCreate(producer_task, "producer_task", 2048, NULL, 5, NULL);
    xTaskCreate(waiter_task, "waiter_task", 2048, NULL, 5, NULL);
}
