# 02_rgb_led_gpio48: GPIO48 RGB 小灯

目标：点亮 GOOUUU ESP32-S3-CAM 板载 GPIO48 RGB/NeoPixel 小灯。

运行效果：

```text
红 -> 绿 -> 蓝 -> 白 -> 灭
```

代码入口：

```text
main/app_main.c
```

运行命令：

```bash
./run.sh
```

退出 monitor：

```text
Ctrl + ]
```

这个示例使用 ESP-IDF 官方 `led_strip` 组件控制 GPIO48 上的一颗 WS2812 RGB 小灯。
