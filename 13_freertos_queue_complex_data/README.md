# 13_freertos_queue_complex_data: FreeRTOS 队列传复杂数据

目标：学习队列传结构体、字符串、数组，以及大数据只传指针。

运行效果：

```text
sender_task 依次发送结构体、字符串、小数组、大字符串指针、大数组指针
receiver_task 收到后根据消息类型解析并打印
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
message_queue = xQueueCreate(5, sizeof(queue_message_t));
xQueueSend(message_queue, &message, portMAX_DELAY);
xQueueReceive(message_queue, &received_message, portMAX_DELAY);
```

一句话理解：

```text
小数据可以直接拷贝进队列，大数据不要整块拷贝，只把指针和长度传给接收任务。
```
