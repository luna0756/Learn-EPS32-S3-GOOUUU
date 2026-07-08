# demo3: FreeRTOS 两个任务 Demo

目标：学习 `xTaskCreate()` 创建多个后台任务。

运行效果：

```text
task_1s 每 1000ms 打印一次
task_500ms 每 500ms 打印一次
```

代码入口：

```text
main/app_main.c
```

常用命令：

```bash
cd /home/stephen/Desktop/esp32/demo3
. ~/esp/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

退出 monitor：

```text
Ctrl + ]
```

学习重点：

```c
xTaskCreate(task_1s, "task_1s", 2048, NULL, 5, NULL);
```

这行代码的意思是：创建一个任务，让它在后台一直运行。
