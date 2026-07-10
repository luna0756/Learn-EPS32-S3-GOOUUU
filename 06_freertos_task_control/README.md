# 06_freertos_task_control: 任务周期、挂起、恢复、删除

目标：学习 `vTaskDelayUntil()`、`vTaskSuspend()`、`vTaskResume()`、`vTaskDelete()`。

运行效果：

```text
periodic_task 使用固定周期运行
controller_task 先挂起它，再恢复它，最后删除它
```

运行命令：

```bash
./run.sh
```
