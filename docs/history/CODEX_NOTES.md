# F571 ESP32-S3-CAM 移植学习记录

记录时间：2026-07-07

## 当前目标

把原始 `F571` ESP-IDF 项目复制出一个实验版，用于在手上的 **GOOUUU ESP32-S3-CAM** 板子上学习、编译、烧录和逐步移植。

实验目录：

```bash
/home/stephen/Desktop/esp32/demo1
```

原始参考目录：

```bash
/home/stephen/Desktop/luna/f571-master/code
```

原始目录先不要改。后续所有实验都在 `demo1` 里做。

## 我们已经知道的事情

原项目是一个 ESP-IDF 灯带控制固件，工程名是 `F571`。

它主要做这些事情：

- 使用 FreeRTOS 多任务。
- 使用 UART 接收外部协议包。
- 使用队列在任务之间传递消息。
- 使用 SPI 模拟 WS2812 灯带时序。
- 支持多路灯带：前灯带、左右灯带、左右三角灯。
- 支持本地按键控制灯效模式。
- 支持 UART OTA 升级。

入口文件是：

```bash
main/app_main.c
```

原始启动流程是：

```c
void app_main(void)
{
    printf("into app_main\n");
    device_spi_init();
    cache_init();
    device_uart_init();
    task_sensor_dectect_init();
    task_app_logic_init();
}
```

## 板子差异

手上的板子写着：

```text
GOOUUU ESP32-S3-CAM
```

它不是普通 ESP32，而是 ESP32-S3。参考资料：

```text
https://github.com/profharris/GOOUUU_ESP32-S3-CAM
```

该板子大致信息：

- 芯片目标应使用 `esp32s3`。
- 常见型号是 ESP32-S3-WROOM-1 N16R8。
- 可能有 16MB Flash、8MB PSRAM。
- 板载 RGB/NeoPixel 常见在 GPIO48。
- USB 串口相关通常是 GPIO43/GPIO44。
- GPIO26 到 GPIO32 和 Flash/PSRAM 相关，不建议当普通 GPIO 使用。

原项目当前 `sdkconfig` 是：

```text
CONFIG_IDF_TARGET="esp32"
```

所以不能直接按原配置烧到 ESP32-S3-CAM。

## 原项目引脚风险

原项目的 SPI 和通道选择引脚在 `components/device/device_spi.h`：

```c
#define PIN_NUM_MOSI1 32
#define PIN_S0 33
#define PIN_S1 25
#define PIN_S2 26
```

这些对 GOOUUU ESP32-S3-CAM 不合适：

- GPIO32：ESP32-S3-CAM 上和 Flash/PSRAM 相关，不建议用。
- GPIO33：ESP32-S3 没有这个 GPIO。
- GPIO25：ESP32-S3 没有这个 GPIO。
- GPIO26：ESP32-S3-CAM 上和 Flash/PSRAM 相关，不建议用。

原项目按键引脚在 `components/key/key.h`：

```c
#define GPIO_KEY1_OPT 13
#define GPIO_KEY2_OPT 15
#define GPIO_KEY3_OPT 4
#define GPIO_KEY4_OPT 17
#define GPIO_KEY5_OPT 21
```

这些 GPIO 在 ESP32-S3-CAM 上可能与摄像头、板载功能或外部接口冲突。学习初期先不要启用按键任务。

## 推荐第一步：做最小启动

先不要跑完整原项目。因为原项目会初始化不适合当前板子的 SPI/GPIO。

第一步目标：

```text
ESP32-S3-CAM 可以编译、烧录、启动，并在 monitor 里看到 printf。
```

进入实验目录：

```bash
cd /home/stephen/Desktop/esp32/demo1
```

加载 ESP-IDF 环境，例如：

```bash
. ~/esp/esp-idf/export.sh
```

切换芯片目标：

```bash
idf.py set-target esp32s3
```

然后修改 `main/app_main.c`，先临时改成：

```c
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("hello GOOUUU ESP32-S3-CAM\n");

    while (1) {
        printf("app is running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

编译：

```bash
idf.py build
```

烧录并看日志：

```bash
idf.py -p /dev/ttyACM0 flash monitor
```

如果串口不是 `/dev/ttyACM0`，先查：

```bash
ls /dev/ttyACM* /dev/ttyUSB*
```

如果烧录失败，可以尝试：

1. 按住 BOOT。
2. 点一下 RST/EN。
3. 松开 BOOT。
4. 重新执行 `idf.py -p ... flash monitor`。

## 推荐第二步：逐步恢复功能

最小启动成功后，再一点点恢复原项目功能：

1. 先恢复 `task_app_logic_init()`，观察任务是否正常。
2. 再看是否需要恢复 `cache_init()`。
3. 暂时不要恢复 `device_spi_init()`，因为原 SPI 引脚不适合 S3-CAM。
4. 暂时不要恢复 `task_sensor_dectect_init()`，因为按键引脚可能与板载摄像头冲突。
5. 如果想控制板载 RGB/NeoPixel，优先研究 GPIO48。
6. 后续再考虑把 WS2812 输出脚从 GPIO32 改到 S3-CAM 上安全可用的 GPIO。

## 学习重点

当前正在从 FreeRTOS 第三阶段开始学：

- `xTaskCreate()`：创建一个后台任务。
- `xQueueCreate()`：创建任务间消息队列。
- `xQueueSend()`：往队列里发消息。
- `xQueueReceive()`：从队列里收消息。

一句话理解：

```text
任务负责同时干活，队列负责任务之间传话。
```

原项目里最重要的一条本地控制链路是：

```text
key.c
  -> task_sensor_dect.c
  -> task_app_logic.c
  -> Modehandle/*.c
  -> display.c
  -> device_spi.c
```

但是在 GOOUUU ESP32-S3-CAM 上，先不要直接跑到 `device_spi.c`，因为引脚需要重新适配。

## 下一次打开 Codex 可以这样说

可以在这个目录里启动 Codex：

```bash
cd /home/stephen/Desktop/esp32/demo1
codex
```

然后告诉 Codex：

```text
请阅读 CODEX_NOTES.md。我们正在把 F571 ESP-IDF 项目移植到 GOOUUU ESP32-S3-CAM。先帮我把 main/app_main.c 改成最小 printf 启动版，然后指导我 set-target esp32s3、build、flash、monitor。
```

## 当前建议

不要急着接外设，也不要急着跑完整灯带程序。先完成这个闭环：

```text
改代码 -> 编译 -> 烧录 -> monitor 看到日志
```

这个闭环跑通之后，再进入任务、队列、GPIO、板载 RGB 灯和 WS2812 的逐步学习。

## GOOUUU ESP32-S3-CAM 的两个 Type-C 口

板子上有两个 Type-C：

```text
TTL
OTG
```

当前阶段建议接 **TTL** 口。

原因：

- `TTL` 口通常接板载 USB 转串口芯片，例如 CH343/CH340 类芯片。
- 这个口连接 ESP32-S3 的 UART0，常见是 GPIO43/GPIO44。
- 用它做 `idf.py flash monitor` 最直接，适合第一次烧录和看日志。
- `OTG` 是 ESP32-S3 的原生 USB 口，常见连接 GPIO19/GPIO20。
- `OTG` 后面可以用于 USB Serial/JTAG、USB Device、USB Host 等实验，但新手阶段先不用它。

推荐连接方式：

```bash
电脑 USB -> 板子 TTL Type-C
```

然后查看串口：

```bash
ls /dev/ttyACM* /dev/ttyUSB*
```

常见情况：

- 如果出现 `/dev/ttyUSB0`，多半是板载 USB 转串口。
- 如果出现 `/dev/ttyACM0`，可能是原生 USB 或某些 CDC 串口。

第一次建议使用 TTL 口对应的串口执行：

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

如果实际枚举成 `/dev/ttyACM0`，就改成：

```bash
idf.py -p /dev/ttyACM0 flash monitor
```

如果烧录失败，尝试手动进入下载模式：

1. 按住 BOOT。
2. 点一下 RST/EN。
3. 松开 BOOT。
4. 重新执行 `idf.py -p ... flash monitor`。

当前不要同时插两个 Type-C，避免串口选择和供电路径混乱。先只插 `TTL`。

## 当前遇到的问题：串口不可读

执行：

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

报错：

```text
Error: Invalid value for '-p' / '--port': Path '/dev/ttyUSB0' is not readable.
```

这通常不是代码问题，而是 Linux 串口权限或串口选择问题。

下一步先执行：

```bash
ls -l /dev/ttyUSB0
groups
```

如果 `/dev/ttyUSB0` 属于 `dialout` 组，而当前用户不在 `dialout`，需要把用户加入串口权限组：

```bash
sudo usermod -aG dialout $USER
```

然后注销重新登录，或者重启电脑，再试：

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

临时验证也可以用：

```bash
sudo chmod a+rw /dev/ttyUSB0
idf.py -p /dev/ttyUSB0 flash monitor
```

如果插拔后串口编号变化，重新查看：

```bash
ls /dev/ttyACM* /dev/ttyUSB*
```

## 当前遇到的问题：芯片目标错误

执行 flash 时曾报错：

```text
A fatal error occurred: This chip is ESP32-S3, not ESP32. Wrong --chip argument?
```

原因：

```text
实验目录里的 sdkconfig/build 仍然是 esp32 目标，esptool 仍在用 --chip esp32。
```

已经执行过：

```bash
cd /home/stephen/Desktop/esp32/demo1
rm -rf build
idf.py set-target esp32s3
```

现在 `sdkconfig` 已经变成：

```text
CONFIG_IDF_TARGET="esp32s3"
```

## 当前临时 CMake 修改

即使 `main/app_main.c` 是最小 printf 版本，ESP-IDF 也会自动编译 `components/` 目录里的组件。

切到 ESP32-S3 后，原始 `components/device/device_spi.c` 编译失败：

```text
error: 'SPI_DMA_CH1' undeclared; did you mean 'SPI_DMA_CH_AUTO'?
```

这是因为原项目驱动代码偏 ESP32，尚未适配 ESP32-S3/ESP-IDF 5.3。

为了先跑通最小启动，已经临时修改：

```bash
components/CMakeLists.txt
```

ESP32-S3 目标下暂时不编译原 `components` 源码，只保留 include 目录：

```cmake
if(CONFIG_IDF_TARGET_ESP32S3)
    idf_component_register(INCLUDE_DIRS ${include_dirs})
else()
    idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS ${include_dirs}
                        REQUIRES driver
                        REQUIRES esp_driver_uart
                        REQUIRES esp_driver_spi
                        REQUIRES esp_driver_gpio
                        REQUIRES esp_event
                        REQUIRES esp_timer
                        REQUIRES app_update
                        )
endif()
```

这只是学习版临时处理。后续恢复功能时，需要逐个适配这些组件：

- `device_spi.c`
- `device_uart.c`
- `key.c`
- `task_sensor_dect.c`
- `task_app_logic.c`

## 当前 build 状态

当前最小 printf 版本已经成功编译。

成功信息包括：

```text
Creating esp32s3 image...
Successfully created esp32s3 image.
Generated .../build/F571.bin
Project build complete.
```

下一步可以烧录：

```bash
cd /home/stephen/Desktop/esp32/demo1
idf.py -p /dev/ttyUSB0 flash monitor
```

如果你的串口不是 `/dev/ttyUSB0`，先查：

```bash
ls /dev/ttyACM* /dev/ttyUSB*
```

烧录成功后，期望在 monitor 里看到：

```text
hello GOOUUU ESP32-S3-CAM
app is running
app is running
```

## 2026-07-07：已裁剪成最小 demo

用户要求：

```text
将工程做整理，将里面的功能记录在一个文档里面，然后将现有的各种功能全部删掉，只保留一个最小能跑的 demo。
```

已经新增功能归档文档：

```text
PROJECT_FEATURES_ARCHIVE.md
```

里面记录了原 F571 工程的主要功能：

- SPI 驱动 WS2812 灯带。
- 多路灯带分区。
- 本地按键扫描。
- FreeRTOS 任务和队列。
- 应用逻辑状态机。
- UART ART 协议。
- UART OTA 升级。

已经从学习版删除原产品功能代码：

```text
components/
main/component.mk
dependencies.lock
README.en.md
Makefile
sdkconfig.old
```

当前学习版只保留最小 demo：

```text
main/app_main.c
main/CMakeLists.txt
CMakeLists.txt
README.md
CODEX_NOTES.md
PROJECT_FEATURES_ARCHIVE.md
sdkconfig
partitions.csv
LICENSE
```

当前 `main/app_main.c` 内容目标：

```text
启动后打印 hello GOOUUU ESP32-S3-CAM，然后每秒打印 app is running。
```

当前 `main/CMakeLists.txt` 已改为 ESP-IDF 5 常用最小写法：

```cmake
idf_component_register(SRCS "app_main.c"
                    INCLUDE_DIRS ".")
```

后续若要重新学习原功能，请不要从当前目录找旧代码，而是去原始参考目录：

```bash
/home/stephen/Desktop/luna/f571-master/code
```

## 裁剪后验证结果

已执行：

```bash
idf.py fullclean
idf.py build
```

验证通过，生成了 ESP32-S3 固件：

```text
Creating esp32s3 image...
Successfully created esp32s3 image.
Generated /home/stephen/Desktop/esp32/demo1/build/F571.bin
Project build complete.
```

当前非 build 文件列表：

```text
PROJECT_FEATURES_ARCHIVE.md
partitions.csv
sdkconfig
main/CMakeLists.txt
LICENSE
main/app_main.c
CODEX_NOTES.md
README.md
CMakeLists.txt
```

当前仍保留 `build/` 目录，因为它是刚刚成功编译生成的固件产物。需要重新干净构建时，可执行：

```bash
idf.py fullclean
idf.py build
```

## 2026-07-07：新增入门教程

已经新增：

```text
GETTING_STARTED.md
```

内容包括：

- 连接 GOOUUU ESP32-S3-CAM 的 `TTL` Type-C。
- 进入项目目录。
- 加载 ESP-IDF 环境。
- 检查 `esp32s3` 目标。
- 执行 `idf.py build`。
- 查找串口。
- 执行 `idf.py -p ... flash monitor`。
- 用 `Ctrl + ]` 退出 monitor。
- 处理串口权限问题。
- 处理芯片目标错误。
- 手动进入下载模式。

## 2026-07-07：目录分版

已经将原实验目录拆成两个版本：

```text
/home/stephen/Desktop/esp32/demo0
/home/stephen/Desktop/esp32/demo1
```

当前目录是 `demo1`：

```text
从 demo0 复制出来，用于继续尝试 GPIO48 板载 RGB/NeoPixel 小灯。
```

`demo0` 保留为：

```text
最小串口打印基线版本。
```

两个目录里的旧 `build/` 缓存都已删除。需要构建时重新执行：

```bash
idf.py build
```

## 2026-07-07：GPIO48 RGB/NeoPixel 小灯 demo

已将 `demo1` 从串口打印 demo 改成 GPIO48 板载 RGB/NeoPixel 小灯 demo。

新增文件：

```text
main/led_strip_encoder.c
main/led_strip_encoder.h
```

修改文件：

```text
main/app_main.c
main/CMakeLists.txt
README.md
CODEX_NOTES.md
```

当前逻辑：

```text
使用 ESP-IDF RMT TX 驱动，在 GPIO48 上发送 WS2812/NeoPixel 时序。
循环显示：红 -> 绿 -> 蓝 -> 白 -> 灭。
```

颜色亮度故意设置较低，例如 `32` 或 `24`，避免板载小灯太刺眼。

如果实际颜色和日志不一致，可能是 RGB 字节顺序和板载灯型号不同。当前发送顺序是 WS2812 常见的 GRB：

```c
uint8_t pixel_grb[3] = {green, red, blue};
```

可以在 `main/app_main.c` 里调整这个数组顺序。
