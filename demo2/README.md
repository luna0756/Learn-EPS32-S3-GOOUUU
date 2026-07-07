# demo2: 普通 GPIO 输出 Demo

目标：学习最基础的 GPIO 输出。

运行效果：

```text
每秒切换一次 GPIO2 的电平：0 -> 1 -> 0 -> 1 ...
串口同步打印当前电平。
```

代码入口：

```text
main/app_main.c
```

常用命令：

```bash
cd /home/stephen/Desktop/esp32/demo2
. ~/esp/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

退出 monitor：

```text
Ctrl + ]
```

如果你接外部 LED，请串联限流电阻。没有外设也没关系，先看串口日志。
