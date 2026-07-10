# 04_freertos_scheduler_note: ESP-IDF 中的调度器启动

目标：理解 `vTaskStartScheduler()` 在 ESP-IDF 中为什么不需要手动调用。

ESP-IDF 已经在系统启动流程中启动 FreeRTOS 调度器，然后才调用 `app_main()`。所以普通 ESP-IDF 应用里不要再调用 `vTaskStartScheduler()`。

运行效果：

```text
app_main 已经运行在 FreeRTOS 调度器之上
```

运行命令：

```bash
./run.sh
```
