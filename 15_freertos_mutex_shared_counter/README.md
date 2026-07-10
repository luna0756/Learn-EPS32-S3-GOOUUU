# 15_freertos_mutex_shared_counter: 互斥锁保护共享变量

目标：学习 `xSemaphoreCreateMutex()`、`xSemaphoreTake()`、`xSemaphoreGive()`。

运行效果：

```text
两个任务同时修改 shared_counter
每次修改前先拿互斥锁，修改后释放互斥锁
```

运行命令：

```bash
./run.sh
```
