# 10_freertos_queue_array: FreeRTOS 队列传数组

目标：学习用队列发送一个小数组。

运行效果：

```text
sender_task 每秒发送一个 int 数组
receiver_task 收到后打印数组内容
```

代码入口：

```text
main/app_main.c
```

运行命令：

```bash
./run.sh
```

学习重点：

```c
array_queue = xQueueCreate(5, sizeof(array_message_t));
xQueueSend(array_queue, &message, portMAX_DELAY);
xQueueReceive(array_queue, &received_message, portMAX_DELAY);
```
