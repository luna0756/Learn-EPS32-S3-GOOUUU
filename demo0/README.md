# demo0: 最小串口 Demo

目标：确认 ESP32-S3-CAM 可以编译、烧录、运行。

运行效果：串口每秒打印一次：

```text
app is running...
```

常用命令：

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

更多教程见：

```text
../docs/tutorials/BUILD_FLASH_MONITOR.md
```
