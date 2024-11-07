# tp_framework

## 这是什么？

**tp_framework**是一个简易的**时间片轮询**裸机框架。支持**任务轮询**以及任务的**挂起**与**恢复**功能。

**时间片轮询**是通过一个基本时钟（**systick** or **timer**），针对不同的任务对 CPU 的处理时间进行时分复用，根据任务列表顺序执行。避免单任务长时间占用 CPU，有一定的实时性 ~~微弱~~。

---

## 使用说明

### 如何将**tp_framework**添加入工程

- 将 tp_framework.c/h 存放到工程路径下。

- 周期性调用 `TPF_Timebase_Int()`函数，如在**systick** or **timer**的中断中。

- 创建 `tp_frame_t` 实例作为框架句柄。

- 创建 `tpf_tasklist_t` 实例作为任务表句柄，在表中填入任务函数及时间片。

- 调用`TPF_Init()`函数，并在最后一个参项位置调用`__GET_TASK_NUMS`宏，初始化框架。

- 调用`TPF_Handler()`函数，进行任务轮询。

- **仔细**
  - 在进行时间片的分配操作前，应估算任务执行时间，过长的任务可能会**淹没**其他任务。
  - 请勿将时序相关操作分割在不同的任务中，**时序敏感**的操作尽量在框架之外运行。
  - 最小的时间片仅支持 1 个时基单位，确保任务轮询的调用**频繁于**时基更新。

### 任务表结构

- **任务序号**：从第一个表成员开始从**0**自增。

- 通过在任务表中创建、删除表成员来增、减任务，表成员的四个参数依次为：
  - 任务函数指针
  - 任务的时间片
  - 任务初始状态
  - 下次运行时间

```c
typedef struct
{
    void (*pff)();
    uint32_t runtick : 31;
    TPF_STATE_E state : 1;
    uint32_t entrytick;
} tpf_tasklist_t;
```

---

## 函数接口与演示

### 函数接口

```
func
  更新时基
  --- TPF_Timebase_Int()

  获取最新时基数
  --- TPF_GetTicks()
    return
      --- newtick         --- 最新时基数

  框架初始化
  --- TPF_Init(tp_frame_t * ,tpf_tasklist_t * ,uint8_t )
    params
      --- pstpfhandle     --- TPF句柄
      --- pstasklist     --- 任务表句柄
      --- tasknum         --- 任务数

  框架事件处理
  --- TPF_Handler(tp_frame_t * )
    params
      --- pstpfhandle     --- TPF句柄

  延时框架内某任务（非阻塞）
  --- TPF_Task_Delay(tp_frame_t * , uint32_t , uint32_t )
    params
      --- pstpfhandle     --- TPF句柄
      --- task_id         --- 任务序号
      --- delay           --- 延时时间

  挂起框架内某任务
  --- TPF_Suspend(tp_frame_t * , uint32_t )
    params
      --- pstpfhandle     --- TPF句柄
      --- task_id         --- 任务序号
    return
      --- 0               --- 一切向好

  恢复框架内某任务
  --- TPF_Resume(tp_frame_t * , uint32_t )
    params
      --- pstpfhandle     --- TPF句柄
      --- task_id         --- 任务序号
    return
      --- 0               --- 一切向好

```

#### TPF_ErrorHandler

当程序进入 `TPF_ErrorHandler()`函数时，可通过查看 `pstasklist` 和 `error` 定位错误。

```
  error
   0  ------------ 检查任务表的指针
  -1  ------------ 检查TPF句柄是否有效
  -2  ------------ 检查任务表句柄是否有效
```

### 演示模板

![image](https://github.com/CuteMan0/tp_framework/blob/main/pic/tpf_demo.gif)

```C
/*----------------------  it.c  ----------------------*/
void SysTick_Handler()//TIMx_IRQHandler()
{
  TPF_Timebase_Int();
}

/*---------------------  main.c  ---------------------*/
static uint8_t task1_flag;

void Task0(void)
{
  static uint32_t t0_cnt;
  task1_flag = t0_cnt;
  printf("T0:%d\r\n", t0_cnt++);
}
void Task1(void)
{
  static uint32_t t1_cnt;
  printf("T1:%d\r\n",t1_cnt++);
}

tp_frame_t tpf;//create a tpf frame and tasklist
tpf_tasklist_t tpf_tasklist[] = {
      {Task0, 1000, TPF_TASK_READY, 0},
      {Task1, 200, TPF_TASK_READY, 0}
//      ...
};

int main(void)
{
  TPF_Init(&tpf, tpf_tasklist, __GET_TASK_NUMS(tpf_tasklist));

  while(1)
  {
    TPF_Handler(&tpf);
    if (10 == task1_flag)
    {
      TPF_Suspend(&tpf, 1);
    }
    if (20 == task1_flag)
    {
      TPF_Resume(&tpf, 1);
    }
  }
}
```
