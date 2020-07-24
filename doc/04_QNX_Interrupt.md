# QNX Interrupt

## 基础概念

CPU中断技术的定义如下:

*  计算机处于执行期间
*  系统内发生了非寻常或非预期的急需处理事件
*  CPU暂时中断当前正在执行的程序而转去执行相应的事件处理程序
*  处理完毕后返回原来被中断处继续执行

中断的优点:

* 可以使CPU和外设同时工作，使系统可以及时地响应外部事件
* 可允许多个外设同时工作，大大提高了CPU的利用率，也提高了数据输入、输出的速度
* 可以使CPU及时处理各种软硬件故障

CPU中断按中断源分为两大类

* 硬件中断 - 外部中断
  * 可屏蔽中断
    * 由计算机的外设或一些接口功能产生，如键盘、打印机、串行口等
    * 可以在CPU要处理其它紧急操作时，被软件屏蔽或忽略
  * 非屏蔽中断
    * 由意外事件导致，如电源断电、内存校验错误等
    * 无法通过软件进行屏蔽，CPU必须无条件响应
* 软件中断 - 内部中断 在程序中调用INTR中断指令引起的中断

在x86架构的处理器中，CPU的中断控制器由两根引脚(INTR和NMI)接收外部中断请求信号。其中：

1. INTR接收可屏蔽中断请求
2. NMI接收非屏蔽中断请求

中断处理

![Interrupt Handling](img/04_intr_handling.png)

其中中断有优先级,导致中断处理也具备了优先级. 
* 高优先级的中断优先处理, 在中断处理的过程遇到更高优先级的中断,当前中断处理暂停,进行更高优先级中断的处理. 
* 相同优先级的中断,按先后顺序依次处理
* 中断的优先级天然高于线程(thread)的优先级

### API

```c

id = InterruptAttach(int intr,
                    struct sigevent *(*handler)(void *,int),
                    void *area, int size,unsigned flags);

id = InterruptAttachEvent(int intr,struct sigevent *event,unsigned flags);

InterruptDetach(int id);
InterruptWait(int flags, uint64_t *reserved);
InterruptMask(int intr,int id);
InterruptUnMask(int intr,int id);

InterruptLock(struct intrspin *spinlock);
InterruptUnlock(struct intrspin *spinlock);
```

使用这些QNX API前需要获得I/O的权限  `ThreadCtl(_NTO_TCTL_IO,0)`