#include <stdio.h>

#include "driver/gpio.h" // 这是 ESP-IDF 的 GPIO 驱动头文件。
#include "esp_log.h" // 用于 ESP-IDF 日志系统。
#include "freertos/FreeRTOS.h" // pdMS_TO_TICKS
#include "freertos/task.h" // vTaskDelay
#include "led_strip.h" // 这是 ESP-IDF 的 LED 灯带组件接口。
#include "led_strip_rmt.h" // 这是 LED strip 使用 RMT 外设的实现相关头文件。

// GOOUUU ESP32-S3-CAM 板载 RGB/NeoPixel 小灯连接到 GPIO48。
#define RGB_LED_GPIO GPIO_NUM_48
#define RGB_LED_COUNT 1

static const char *TAG = "rgb_led_gpio48";

static void set_rgb(led_strip_handle_t led, uint8_t red, uint8_t green, uint8_t blue)
{
    // WS2812 需要先写入像素颜色，再 refresh 才会真正输出到灯上。
    ESP_ERROR_CHECK(led_strip_set_pixel(led, 0, red, green, blue));
    ESP_ERROR_CHECK(led_strip_refresh(led));
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM GPIO48 RGB demo\n");

    // 配置灯带使用哪个 GPIO、灯珠数量和颜色顺序。
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_GPIO,
        .max_leds = RGB_LED_COUNT,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };

    // RMT 是 ESP32 用来产生 WS2812 精确时序的外设。
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000,
    };

    led_strip_handle_t led = NULL;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led));
    ESP_LOGI(TAG, "GPIO48 RGB LED example started");

    while (1) {
        ESP_LOGI(TAG, "red");
        set_rgb(led, 32, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(700));

        ESP_LOGI(TAG, "green");
        set_rgb(led, 0, 32, 0);
        vTaskDelay(pdMS_TO_TICKS(700));

        ESP_LOGI(TAG, "blue");
        set_rgb(led, 0, 0, 32);
        vTaskDelay(pdMS_TO_TICKS(700));

        ESP_LOGI(TAG, "white");
        set_rgb(led, 24, 24, 24);
        vTaskDelay(pdMS_TO_TICKS(700));

        ESP_LOGI(TAG, "off");
        set_rgb(led, 0, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(700));
    }
}
