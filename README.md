# ESP32-S3 学习目录

当前目录用于学习 GOOUUU ESP32-S3-CAM。

## 目录

```text
demo0/   最小串口打印 demo
demo1/   GPIO48 RGB/NeoPixel 小灯 demo
demo2/   普通 GPIO 输出 demo
demo3/   FreeRTOS 两个任务 demo
demo4/   FreeRTOS 队列 demo
docs/    教程、归档和历史记录
```

## 推荐学习顺序

1. 先跑 `demo0`，确认 build/flash/monitor 流程。
2. 再跑 `demo1`，确认 GPIO48 RGB 小灯可以控制。
3. 再跑 `demo2`，学习普通 GPIO 输出。
4. 再跑 `demo3`，学习 FreeRTOS 创建多个任务。
5. 再跑 `demo4`，学习 FreeRTOS 队列传递消息。
6. 之后每做一个新实验，复制一个新目录，例如 `demo5`。

## 常用命令

```bash
cd /home/stephen/Desktop/esp32/demo0
. ~/esp/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

退出 monitor：

```text
Ctrl + ]
```

完整教程：

```text
docs/tutorials/BUILD_FLASH_MONITOR.md
```

原 F571 项目功能归档：

```text
docs/archive/F571_ORIGINAL_FEATURES.md
```
