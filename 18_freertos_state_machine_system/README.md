# 18_freertos_state_machine_system: FreeRTOS 状态机综合项目

目标：把任务、队列、按键中断、串口事件、信号量、互斥锁、事件组、软件定时器和状态机串起来。

运行效果：

```text
BOOT(GPIO0) 按键产生 BUTTON 事件
串口输入 s/p/e/r 产生 START/PAUSE/ERROR/RESET 事件
state_machine_task 从队列接收事件并切换状态
led_task 用 GPIO2 显示不同状态
status_timer 定时产生上报事件
status_task 打印当前状态和事件组变化
```

串口命令：

```text
s  START：进入 RUNNING
p  PAUSE：进入 PAUSED
e  ERROR：进入 ERROR
r  RESET：回到 IDLE
```

状态对应 GPIO2：

```text
IDLE    LED 熄灭
RUNNING LED 快闪
PAUSED  LED 慢闪
ERROR   LED 常亮
```

运行命令：

```bash
./run.sh
```

注意：BOOT 键通常连接 GPIO0，GPIO2 可外接 LED 观察状态。
