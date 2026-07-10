# 14_freertos_binary_semaphore_button_isr: 按键中断和二值信号量

目标：学习 `xSemaphoreCreateBinary()`、`xSemaphoreGiveFromISR()`、`xSemaphoreTake()`、`portYIELD_FROM_ISR()`。

运行效果：

```text
按下 BOOT(GPIO0) 后，中断释放二值信号量
button_task 被唤醒并打印按键次数
```

运行命令：

```bash
./run.sh
```

注意：BOOT 键通常连接 GPIO0。不同开发板如果按键引脚不同，修改 `BUTTON_GPIO`。
