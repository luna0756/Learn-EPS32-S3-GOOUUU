# 03_gpio_output: 普通 GPIO 输出

目标：学习最基础的 GPIO 输出。

运行效果：

```text
每秒切换一次 GPIO2 的电平：0 -> 1 -> 0 -> 1 ...
串口同步打印当前电平。
```

代码入口：

```text
main/app_main.c
```

运行命令：

```bash
./run.sh
```

退出 monitor：

```text
Ctrl + ]
```

如果你接外部 LED，请串联限流电阻。没有外设也没关系，可以先看串口日志。
