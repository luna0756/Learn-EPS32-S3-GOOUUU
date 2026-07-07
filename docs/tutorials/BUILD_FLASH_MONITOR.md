# Build / Flash / Monitor 小教程

适用于：

```text
demo0
demo1
```

## 1. 接线

连接板子的 `TTL` Type-C 口：

```text
电脑 USB -> GOOUUU ESP32-S3-CAM TTL
```

先不要同时接 `TTL` 和 `OTG`。

## 2. 进入 demo 目录

例如：

```bash
cd /home/stephen/Desktop/esp32/demo0
```

或：

```bash
cd /home/stephen/Desktop/esp32/demo1
```

## 3. 加载 ESP-IDF

每开一个新终端都执行一次：

```bash
. ~/esp/esp-idf/export.sh
```

## 4. 编译

```bash
idf.py build
```

需要彻底重新编译时：

```bash
idf.py fullclean
idf.py build
```

## 5. 查串口

```bash
ls /dev/ttyACM* /dev/ttyUSB*
```

常见是：

```text
/dev/ttyUSB0
```

## 6. 烧录并看日志

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

如果你的串口是 `/dev/ttyACM0`：

```bash
idf.py -p /dev/ttyACM0 flash monitor
```

## 7. 退出 monitor

```text
Ctrl + ]
```

## 8. 常见问题

串口没有权限：

```bash
sudo usermod -aG dialout $USER
```

然后注销重新登录，或重启电脑。

临时授权：

```bash
sudo chmod a+rw /dev/ttyUSB0
```

芯片目标错误时：

```bash
rm -rf build
idf.py set-target esp32s3
idf.py build
```

烧录失败时，可以手动进入下载模式：

1. 按住 `BOOT`。
2. 点一下 `RST/EN`。
3. 松开 `BOOT`。
4. 重新执行 flash 命令。
