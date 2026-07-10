#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

static void report_timer_callback(TimerHandle_t timer)
{
    static int report_count = 0;

    (void)timer;
    printf("report_timer: periodic report %d\n", ++report_count);
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS software timer demo\n");

    TimerHandle_t report_timer = xTimerCreate(
        "report_timer",
        pdMS_TO_TICKS(2000),
        pdTRUE,   // pdTRUE 表示自动重装，也就是周期定时器。
        NULL,
        report_timer_callback);

    if (report_timer == NULL) {
        printf("failed to create timer\n");
        return;
    }

    xTimerStart(report_timer, portMAX_DELAY);
}
