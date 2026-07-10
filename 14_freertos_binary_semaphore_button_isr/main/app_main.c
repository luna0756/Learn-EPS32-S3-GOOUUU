#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define BUTTON_GPIO GPIO_NUM_0

static SemaphoreHandle_t button_semaphore;

static void IRAM_ATTR button_isr_handler(void *arg)
{
    (void)arg;

    BaseType_t high_priority_task_woken = pdFALSE;

    // ISR 中不能调用普通 xSemaphoreGive，要用 FromISR 版本。
    xSemaphoreGiveFromISR(button_semaphore, &high_priority_task_woken);
    portYIELD_FROM_ISR(high_priority_task_woken);
}

static void button_task(void *arg)
{
    (void)arg;

    int press_count = 0;

    while (1) {
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("button_task: BOOT button pressed %d times\n", ++press_count);
        }
    }
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS binary semaphore button ISR demo\n");

    button_semaphore = xSemaphoreCreateBinary();
    if (button_semaphore == NULL) {
        printf("failed to create semaphore\n");
        return;
    }

    gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&button_config));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL));

    xTaskCreate(button_task, "button_task", 2048, NULL, 5, NULL);
}
