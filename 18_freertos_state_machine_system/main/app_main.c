#include <stdio.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define BUTTON_GPIO GPIO_NUM_0
#define LED_GPIO GPIO_NUM_2
#define UART_PORT UART_NUM_0
#define UART_RX_BUFFER_SIZE 1024

#define STATE_CHANGED_BIT BIT0
#define ERROR_STATE_BIT BIT1
#define TIMER_REPORT_BIT BIT2

typedef enum {
    APP_STATE_IDLE = 0,
    APP_STATE_RUNNING,
    APP_STATE_PAUSED,
    APP_STATE_ERROR,
} app_state_t;

typedef enum {
    APP_EVENT_BUTTON = 0,
    APP_EVENT_UART_START,
    APP_EVENT_UART_PAUSE,
    APP_EVENT_UART_ERROR,
    APP_EVENT_UART_RESET,
    APP_EVENT_TIMER_REPORT,
} app_event_type_t;

typedef struct {
    app_event_type_t type;
    char source;
} app_event_t;

static QueueHandle_t event_queue;
static SemaphoreHandle_t button_semaphore;
static SemaphoreHandle_t state_mutex;
static EventGroupHandle_t system_event_group;
static TimerHandle_t status_timer;

static app_state_t current_state = APP_STATE_IDLE;

static const char *state_to_string(app_state_t state)
{
    switch (state) {
    case APP_STATE_IDLE:
        return "IDLE";
    case APP_STATE_RUNNING:
        return "RUNNING";
    case APP_STATE_PAUSED:
        return "PAUSED";
    case APP_STATE_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

static app_state_t get_current_state(void)
{
    app_state_t state;

    xSemaphoreTake(state_mutex, portMAX_DELAY);
    state = current_state;
    xSemaphoreGive(state_mutex);

    return state;
}

static void set_current_state(app_state_t new_state)
{
    xSemaphoreTake(state_mutex, portMAX_DELAY);
    current_state = new_state;
    xSemaphoreGive(state_mutex);

    EventBits_t bits = STATE_CHANGED_BIT;
    if (new_state == APP_STATE_ERROR) {
        bits |= ERROR_STATE_BIT;
    }

    xEventGroupSetBits(system_event_group, bits);
}

static void IRAM_ATTR button_isr_handler(void *arg)
{
    (void)arg;

    BaseType_t high_priority_task_woken = pdFALSE;
    app_event_t event = {
        .type = APP_EVENT_BUTTON,
        .source = 'B',
    };

    // 按键中断同时演示 FromISR 版队列发送和信号量释放。
    xQueueSendFromISR(event_queue, &event, &high_priority_task_woken);
    xSemaphoreGiveFromISR(button_semaphore, &high_priority_task_woken);
    portYIELD_FROM_ISR(high_priority_task_woken);
}

static void status_timer_callback(TimerHandle_t timer)
{
    (void)timer;

    app_event_t event = {
        .type = APP_EVENT_TIMER_REPORT,
        .source = 'T',
    };

    xQueueSend(event_queue, &event, 0);
    xEventGroupSetBits(system_event_group, TIMER_REPORT_BIT);
}

static void button_task(void *arg)
{
    (void)arg;

    int press_count = 0;

    while (1) {
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY) == pdTRUE) {
            printf("button_task: BOOT pressed, count=%d\n", ++press_count);
        }
    }
}

static void uart_task(void *arg)
{
    (void)arg;

    uint8_t byte;

    while (1) {
        int length = uart_read_bytes(UART_PORT, &byte, 1, pdMS_TO_TICKS(100));
        if (length <= 0) {
            continue;
        }

        app_event_t event = {
            .source = (char)byte,
        };

        switch (byte) {
        case 's':
        case 'S':
            event.type = APP_EVENT_UART_START;
            break;
        case 'p':
        case 'P':
            event.type = APP_EVENT_UART_PAUSE;
            break;
        case 'e':
        case 'E':
            event.type = APP_EVENT_UART_ERROR;
            break;
        case 'r':
        case 'R':
            event.type = APP_EVENT_UART_RESET;
            break;
        default:
            printf("uart_task: unknown command '%c', use s/p/e/r\n", byte);
            continue;
        }

        printf("uart_task: command '%c' -> queue event\n", byte);
        xQueueSend(event_queue, &event, portMAX_DELAY);
    }
}

static void state_machine_task(void *arg)
{
    (void)arg;

    app_event_t event;

    while (1) {
        if (xQueueReceive(event_queue, &event, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        app_state_t old_state = get_current_state();
        app_state_t new_state = old_state;

        switch (event.type) {
        case APP_EVENT_BUTTON:
            new_state = (old_state == APP_STATE_RUNNING) ? APP_STATE_PAUSED : APP_STATE_RUNNING;
            break;
        case APP_EVENT_UART_START:
            new_state = APP_STATE_RUNNING;
            break;
        case APP_EVENT_UART_PAUSE:
            new_state = APP_STATE_PAUSED;
            break;
        case APP_EVENT_UART_ERROR:
            new_state = APP_STATE_ERROR;
            break;
        case APP_EVENT_UART_RESET:
            new_state = APP_STATE_IDLE;
            break;
        case APP_EVENT_TIMER_REPORT:
            printf("state_machine_task: timer report, current state=%s\n", state_to_string(old_state));
            break;
        default:
            break;
        }

        if (new_state != old_state) {
            printf("state_machine_task: %s -> %s, source=%c\n",
                   state_to_string(old_state),
                   state_to_string(new_state),
                   event.source);
            set_current_state(new_state);
        }
    }
}

static void led_task(void *arg)
{
    (void)arg;

    int blink_level = 0;

    while (1) {
        switch (get_current_state()) {
        case APP_STATE_IDLE:
            gpio_set_level(LED_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(300));
            break;
        case APP_STATE_RUNNING:
            blink_level = !blink_level;
            gpio_set_level(LED_GPIO, blink_level);
            vTaskDelay(pdMS_TO_TICKS(150));
            break;
        case APP_STATE_PAUSED:
            blink_level = !blink_level;
            gpio_set_level(LED_GPIO, blink_level);
            vTaskDelay(pdMS_TO_TICKS(700));
            break;
        case APP_STATE_ERROR:
            gpio_set_level(LED_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(300));
            break;
        default:
            vTaskDelay(pdMS_TO_TICKS(300));
            break;
        }
    }
}

static void status_task(void *arg)
{
    (void)arg;

    TickType_t last_wake_time = xTaskGetTickCount();

    while (1) {
        EventBits_t bits = xEventGroupWaitBits(
            system_event_group,
            STATE_CHANGED_BIT | ERROR_STATE_BIT | TIMER_REPORT_BIT,
            pdTRUE,
            pdFALSE,
            pdMS_TO_TICKS(1000));

        if (bits != 0) {
            printf("status_task: event bits=0x%02x, state=%s\n",
                   (unsigned int)bits,
                   state_to_string(get_current_state()));
        }

        // 固定周期心跳，演示 vTaskDelayUntil。
        printf("status_task: heartbeat state=%s\n", state_to_string(get_current_state()));
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(2000));
    }
}

static void init_gpio(void)
{
    gpio_config_t led_config = {
        .pin_bit_mask = 1ULL << LED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led_config));

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
}

static void init_uart(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RX_BUFFER_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
}

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM FreeRTOS state machine system demo\n");
    printf("serial commands: s=start, p=pause, e=error, r=reset\n");

    event_queue = xQueueCreate(10, sizeof(app_event_t));
    button_semaphore = xSemaphoreCreateBinary();
    state_mutex = xSemaphoreCreateMutex();
    system_event_group = xEventGroupCreate();

    if (event_queue == NULL || button_semaphore == NULL ||
        state_mutex == NULL || system_event_group == NULL) {
        printf("failed to create FreeRTOS objects\n");
        return;
    }

    init_gpio();
    init_uart();

    status_timer = xTimerCreate(
        "status_timer",
        pdMS_TO_TICKS(5000),
        pdTRUE,
        NULL,
        status_timer_callback);
    if (status_timer == NULL) {
        printf("failed to create status timer\n");
        return;
    }

    xTaskCreate(button_task, "button_task", 2048, NULL, 6, NULL);
    xTaskCreate(uart_task, "uart_task", 3072, NULL, 6, NULL);
    xTaskCreate(state_machine_task, "state_machine", 3072, NULL, 7, NULL);
    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
    xTaskCreate(status_task, "status_task", 3072, NULL, 5, NULL);

    xTimerStart(status_timer, portMAX_DELAY);
}
