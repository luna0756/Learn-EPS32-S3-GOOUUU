# 08_freertos_queue_struct: FreeRTOS 队列传结构体

目标：学习用队列发送一个结构体。

运行效果：

```text
sender_task 每秒发送一个 sensor_data_t 结构体
receiver_task 收到后打印结构体字段
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
data_queue = xQueueCreate(5, sizeof(sensor_data_t));
xQueueSend(data_queue, &data, portMAX_DELAY);
xQueueReceive(data_queue, &received_data, portMAX_DELAY);
```
