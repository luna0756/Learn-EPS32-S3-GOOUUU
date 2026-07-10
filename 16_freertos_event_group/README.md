# 16_freertos_event_group: 事件组

目标：学习 `xEventGroupCreate()`、`xEventGroupSetBits()`、`xEventGroupWaitBits()`。

运行效果：

```text
producer_task 依次设置 WIFI_READY 和 MQTT_READY 两个事件位
waiter_task 等两个事件位都到齐后继续执行
```

运行命令：

```bash
./run.sh
```
