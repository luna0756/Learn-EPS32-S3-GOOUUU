# 09_freertos_queue_string: FreeRTOS 队列传字符串

目标：学习用队列发送短字符串。

运行效果：

```text
sender_task 每秒发送一个字符串数组
receiver_task 收到后打印字符串内容
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
text_queue = xQueueCreate(5, sizeof(text_message_t));
xQueueSend(text_queue, &message, portMAX_DELAY);
xQueueReceive(text_queue, &received_message, portMAX_DELAY);
```
