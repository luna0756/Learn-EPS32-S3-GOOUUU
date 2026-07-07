# 原 F571 工程功能归档

本文档记录本目录在裁剪成最小 demo 之前，原始 F571 固件包含的主要功能。代码已作为学习版精简，原始完整工程仍保留在：

```bash
/home/stephen/Desktop/luna/f571-master/code
```

## 项目定位

原 F571 是一个基于 ESP-IDF 的 ESP32 灯带控制固件，主要面向多路 WS2812 灯带产品。

它不是普通单文件示例，而是包含 FreeRTOS 多任务、UART 协议解析、SPI 灯带输出、本地按键、灯效状态机和 OTA 升级的产品型固件。

## 原始启动流程

原入口文件 `main/app_main.c` 会依次初始化：

```c
device_spi_init();
cache_init();
device_uart_init();
task_sensor_dectect_init();
task_app_logic_init();
```

含义如下：

- `device_spi_init()`：初始化 SPI 输出任务，用于驱动 WS2812。
- `cache_init()`：初始化 UART 接收缓存和拼包任务。
- `device_uart_init()`：初始化 UART1 通信。
- `task_sensor_dectect_init()`：初始化按键扫描任务。
- `task_app_logic_init()`：初始化应用逻辑状态机任务。

## 灯带控制

原项目通过 SPI 模拟 WS2812 时序。

关键文件：

```text
components/device/device_spi.c
components/device/device_spi.h
components/display.c
components/display.h
```

原项目灯带分组：

```c
#define FRONT_PORT        3
#define RIGHT_PORT        0
#define LEFT_PORT         1
#define CORNER_RIGHT_PORT 2
#define CORNER_LEFT_PORT  4

#define FRONT_LIGHT   118
#define AROUND_LIGHT   61
#define CORNER_LIGHT   12
```

也就是：

- 前灯带：118 颗
- 右灯带：61 颗
- 左灯带：61 颗
- 右三角灯：12 颗
- 左三角灯：12 颗

原 ESP32 版本使用了这些 SPI/通道选择引脚：

```c
#define PIN_NUM_MOSI1 32
#define PIN_S0        33
#define PIN_S1        25
#define PIN_S2        26
```

这些引脚不适合 GOOUUU ESP32-S3-CAM，因此学习版不再保留原驱动代码。

## 本地按键

关键文件：

```text
components/key/key.c
components/key/key.h
components/task_sensor_dect.c
components/task_sensor_dect.h
```

原项目支持 5 个按键：

```c
#define GPIO_KEY1_OPT 13
#define GPIO_KEY2_OPT 15
#define GPIO_KEY3_OPT 4
#define GPIO_KEY4_OPT 17
#define GPIO_KEY5_OPT 21
```

按键支持短按和 2 秒长按。

按键任务会扫描 GPIO，把动作封装成消息，通过 FreeRTOS 队列发给应用逻辑任务。

## FreeRTOS 任务和队列

原项目使用 FreeRTOS 多任务组织功能。

核心概念：

- `xTaskCreate()`：创建后台任务。
- `xQueueCreate()`：创建队列。
- `xQueueSend()`：向队列发消息。
- `xQueueReceive()`：从队列收消息。

典型链路：

```text
key.c
  -> task_sensor_dect.c
  -> task_app_logic.c
  -> Modehandle/*.c
  -> display.c
  -> device_spi.c
```

## 应用逻辑状态机

关键文件：

```text
components/task_app_logic.c
components/task_app_logic.h
components/Modehandle/mode_front_handle.c
components/Modehandle/mode_around_handle.c
components/Modehandle/mode_corner_handle.c
```

系统状态枚举包括：

```c
SSP_POWER_OFF
SSP_LOGO
SSP_MODE_1
SSP_MODE_2
SSP_MODE_3
SSP_MODE_4
SSP_MODE_5
```

应用操作枚举包括：

```c
OPT_NONE
OPT_POWER_TURNON
OPT_POWER_TURNOFF
OPT_GO_NEXT
OPT_INVALID
```

含义是：

- 当前灯组处于哪个模式。
- 用户操作后应该开灯、关灯，还是进入下一个模式。

## UART 协议

关键文件：

```text
components/device/device_uart.c
components/device/device_uart.h
components/cache_handle.c
components/cache_handle.h
components/cloud_attribute.c
components/cloud_attribute.h
components/report.c
components/report.h
```

原项目使用 UART1，配置过的参数包括：

```c
#define UART_RATE    2 * 1000 * 1000
#define UART_NUM     UART_NUM_1
#define UART1_RX_PIN 14
#define UART1_TX_PIN 27
```

协议包头为：

```text
0x41 0x52 0x54
```

也就是 ASCII：

```text
ART
```

包尾使用连续三个 `0x0A` 作为结束标记。

支持的协议命令包括：

- 发现设备
- 注册回包
- 进入高速灯效模式
- 高速灯效数据
- 退出高速模式
- 开始 OTA
- OTA 数据下载
- 结束 OTA

## OTA 升级

关键文件：

```text
components/OTA_task.c
components/OTA_task.h
```

原项目通过 UART 协议接收 OTA 数据，然后调用 ESP-IDF OTA API：

- `esp_ota_get_next_update_partition()`
- `esp_ota_begin()`
- `esp_ota_write()`
- `esp_ota_end()`
- `esp_ota_set_boot_partition()`

## 为什么学习版要删除这些功能

当前硬件是 GOOUUU ESP32-S3-CAM，不是原项目面向的普通 ESP32 灯带控制板。

主要差异：

- 芯片目标从 `esp32` 改为 `esp32s3`。
- 原项目部分 GPIO 在 ESP32-S3-CAM 上不存在或与 Flash/PSRAM/摄像头冲突。
- 原 SPI 驱动里使用了 ESP32 相关宏，例如 `SPI_DMA_CH1`，在当前 ESP32-S3/ESP-IDF 5.3 环境下不能直接编译。
- 初学阶段最重要的是先跑通 `编译 -> 烧录 -> monitor 日志` 闭环。

因此学习版暂时只保留最小 demo。

## 当前最小 demo 的目标

当前学习版只做一件事：

```text
启动后每秒通过串口打印 app is running。
```

目标文件：

```text
main/app_main.c
```

后续可以在这个干净基础上逐步加入：

1. 一个 FreeRTOS 任务。
2. 一个 GPIO 输出。
3. 板载 GPIO48 RGB/NeoPixel。
4. 一个 FreeRTOS 队列。
5. 再回头移植原项目的按键、状态机和灯带逻辑。
