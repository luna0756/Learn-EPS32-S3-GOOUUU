

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM\n");

    while (1) {
        printf("app is running...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
