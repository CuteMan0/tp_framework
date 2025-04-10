# tp_framework

## 这是什么？

**tp_framework**是一个简易的**时间片轮询**裸机框架。支持**任务轮询**以及任务的**挂起**与**恢复**功能。

**时间片轮询**是通过一个基本时钟（**systick** or **timer**），针对不同的任务对 CPU 的处理时间进行时分复用，根据任务列表顺序执行。避免单任务长时间占用 CPU，有一定的实时性 ~~微弱~~。

---

## 使用说明

### 如何将**tp_framework**添加入工程

- 将 tp_framework.c/h 存放到工程路径下。

- 调用`Scheduler_Init()`函数，初始化框架调度器。

- 周期性调用 `Scheduler_TickUpdate()`函数，如在**systick** or **timer**的中断中。

- 高频次调用`Scheduler_Run()`函数，进行任务轮询，如在**while** or **loop**中。

- **仔细**
  - 过长的任务可能会**淹没**其他任务，提供`Scheduler_SetTimeoutCallback()`回调接口，处理超时异常。
  - 请勿将时序相关操作分割在不同的任务中，**时序敏感**的操作尽量在框架之外运行。
  - 最小的时间片仅支持 1 个时基单位，确保任务轮询的调用**频繁于**时基更新。

---

## 演示模板

![image](https://github.com/CuteMan0/tp_framework/blob/main/pic/tpf_demo.gif)

```C
/*----------------------  it.c  ----------------------*/
void SysTick_Handler(void)
{
  Scheduler_TickUpdate();
}

/*---------------------  main.c  ---------------------*/
volatile uint32_t t0_cnt = 0, t1_cnt = 0;

void Task0(void)
{
  printf("T0:%d\r\n", t0_cnt++);
  t0_cnt++;
}
void Task1(void)
{
  printf("T1:%d\r\n",t1_cnt++);
}

// void TimeoutHandler(int task_id)
// {
// }

int main(void)
{

Scheduler_Init();
// Scheduler_SetTimeoutCallback(TimeoutHandler);

int t0 = Scheduler_AddTask(Task0, 500, 0);
int t1 = Scheduler_AddTask(Task1, 500, 0);

  while(1)
  {
    Scheduler_Run();
    if (10 == t0_cnt)
    {
      Scheduler_SuspendTask(t1);
    }
    if (20 == t0_cnt)
    {
      Scheduler_ResumeTask(t1);
    }
  }
}
```
