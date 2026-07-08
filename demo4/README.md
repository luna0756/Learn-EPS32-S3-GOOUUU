# demo4: FreeRTOS 队列 Demo

目标：学习 `xQueueCreate()`、`xQueueSend()`、`xQueueReceive()`。

运行效果：

```text
sender_task 每秒发送一个递增数字
receiver_task 收到数字后打印
```

代码入口：

```text
main/app_main.c
```

常用命令：

```bash
cd /home/stephen/Desktop/esp32/demo4
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
number_queue = xQueueCreate(5, sizeof(int));
xQueueSend(number_queue, &number, portMAX_DELAY);
xQueueReceive(number_queue, &received_number, portMAX_DELAY);
```

一句话理解：

```text
发送任务把数字放进队列，接收任务从队列里取数字。
```
