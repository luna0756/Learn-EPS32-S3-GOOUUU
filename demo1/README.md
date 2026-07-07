# demo1: GPIO48 RGB 小灯 Demo

目标：点亮 GOOUUU ESP32-S3-CAM 板载 GPIO48 RGB/NeoPixel 小灯。

运行效果：

```text
红 -> 绿 -> 蓝 -> 白 -> 灭
```

常用命令：

```bash
cd /home/stephen/Desktop/esp32/demo1
. ~/esp/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

这个版本只需要看一个代码文件：

```text
main/app_main.c
```

它使用 ESP-IDF 官方 `led_strip` 组件控制 GPIO48 上的一颗 WS2812 RGB 小灯。

更多教程见：

```text
../docs/tutorials/BUILD_FLASH_MONITOR.md
```
