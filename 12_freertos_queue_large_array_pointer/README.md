# 12_freertos_queue_large_array_pointer: FreeRTOS 队列传大数组指针

目标：学习大数组不拷贝进队列，只传指针和长度。

运行效果：

```text
sender_task 每秒发送 large_array 的地址和长度
receiver_task 收到后通过指针读取数组
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
ptr_queue = xQueueCreate(5, sizeof(array_ptr_message_t));
xQueueSend(ptr_queue, &message, portMAX_DELAY);
xQueueReceive(ptr_queue, &received_message, portMAX_DELAY);
```

注意：只传指针时，指针指向的数据必须一直有效，不能指向发送函数里的局部数组。
