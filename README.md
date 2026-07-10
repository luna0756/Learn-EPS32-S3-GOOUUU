# ESP32-S3 FreeRTOS 学习目录

当前目录用于学习 GOOUUU ESP32-S3-CAM。每个示例都是一个独立 ESP-IDF 工程，目录名前面的数字就是推荐学习顺序；基础示例尽量“一次只讲一个概念”，需要联动的功能放在综合状态机项目里。

## 推荐学习顺序

```text
01_minimal_serial                         最小串口打印，确认环境可用
02_rgb_led_gpio48                         GPIO48 RGB/NeoPixel 小灯
03_gpio_output                            普通 GPIO 输出
04_freertos_scheduler_note                ESP-IDF 中调度器已经自动启动
05_freertos_tasks                         创建多个任务
06_freertos_task_control                  周期延时、挂起、恢复、删除任务
07_freertos_queue_number                  队列传 int
08_freertos_queue_struct                  队列传结构体
09_freertos_queue_string                  队列传字符串
10_freertos_queue_array                   队列传数组
11_freertos_queue_large_string_pointer    大字符串只传指针
12_freertos_queue_large_array_pointer     大数组只传指针
13_freertos_queue_complex_data            多种队列消息综合示例
14_freertos_binary_semaphore_button_isr   按键中断释放二值信号量
15_freertos_mutex_shared_counter          互斥锁保护共享变量
16_freertos_event_group                   事件组同步多个条件
17_freertos_software_timer                软件定时器
18_freertos_state_machine_system          FreeRTOS + 事件队列 + 状态机综合项目
```

## 运行方式

每个示例目录都有 `run.sh`：

```bash
cd /home/stephen/Desktop/esp32/18_freertos_state_machine_system
./run.sh
```

默认串口是 `/dev/ttyUSB0`。如果设备在其他串口：

```bash
PORT=/dev/ttyACM0 ./run.sh
```

退出 monitor：

```text
Ctrl + ]
```

## API 覆盖表

| FreeRTOS API | 示例 |
| --- | --- |
| `xTaskCreate()` | `05_freertos_tasks`, `18_freertos_state_machine_system` |
| `vTaskStartScheduler()` | `04_freertos_scheduler_note` 说明 ESP-IDF 已自动调用，不在应用里重复调用 |
| `vTaskDelay()` | 多数基础示例 |
| `vTaskDelayUntil()` | `06_freertos_task_control`, `18_freertos_state_machine_system` |
| `vTaskDelete()` | `06_freertos_task_control` |
| `vTaskSuspend()` | `06_freertos_task_control` |
| `vTaskResume()` | `06_freertos_task_control` |
| `xQueueCreate()` | `07_freertos_queue_number`, `18_freertos_state_machine_system` |
| `xQueueSend()` | 队列系列示例，`18_freertos_state_machine_system` |
| `xQueueReceive()` | 队列系列示例，`18_freertos_state_machine_system` |
| `xSemaphoreCreateBinary()` | `14_freertos_binary_semaphore_button_isr`, `18_freertos_state_machine_system` |
| `xSemaphoreGive()` | `15_freertos_mutex_shared_counter`, `18_freertos_state_machine_system` |
| `xSemaphoreTake()` | `14_freertos_binary_semaphore_button_isr`, `15_freertos_mutex_shared_counter`, `18_freertos_state_machine_system` |
| `xSemaphoreCreateMutex()` | `15_freertos_mutex_shared_counter`, `18_freertos_state_machine_system` |
| `xEventGroupCreate()` | `16_freertos_event_group`, `18_freertos_state_machine_system` |
| `xEventGroupSetBits()` | `16_freertos_event_group`, `18_freertos_state_machine_system` |
| `xEventGroupWaitBits()` | `16_freertos_event_group`, `18_freertos_state_machine_system` |
| `xTimerCreate()` | `17_freertos_software_timer`, `18_freertos_state_machine_system` |
| `xTimerStart()` | `17_freertos_software_timer`, `18_freertos_state_machine_system` |
| `xQueueSendFromISR()` | `18_freertos_state_machine_system` |
| `xSemaphoreGiveFromISR()` | `14_freertos_binary_semaphore_button_isr`, `18_freertos_state_machine_system` |
| `portYIELD_FROM_ISR()` | `14_freertos_binary_semaphore_button_isr`, `18_freertos_state_machine_system` |

## 综合项目功能表

| 功能 | 是否完成 | 示例 |
| --- | --- | --- |
| FreeRTOS 正常启动 | 完成 | ESP-IDF 自动启动，见 `04_freertos_scheduler_note` 和所有示例 |
| 创建不少于 4 个任务 | 完成 | `18_freertos_state_machine_system` 创建 5 个任务 |
| 按键产生事件 | 完成 | `18_freertos_state_machine_system` 的 BOOT(GPIO0) ISR |
| 串口产生事件 | 完成 | `18_freertos_state_machine_system` 输入 `s/p/e/r` |
| 状态机接收事件 | 完成 | `state_machine_task` 从队列接收事件 |
| LED 显示不同状态 | 完成 | GPIO2 显示 IDLE/RUNNING/PAUSED/ERROR |
| 队列通信 | 完成 | `event_queue` |
| 至少一个错误状态 | 完成 | `APP_STATE_ERROR` |
| 串口打印当前状态 | 完成 | `status_task` 和状态切换日志 |
| 软件定时器定时上报 | 完成 | `status_timer` |

完整教程：

```text
docs/tutorials/BUILD_FLASH_MONITOR.md
```

原 F571 项目功能归档：

```text
docs/archive/F571_ORIGINAL_FEATURES.md
```
