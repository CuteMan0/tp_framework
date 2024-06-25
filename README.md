# tp_framework_demo

## 这是什么？

**tp_framework_demo**是一个简易的**时间片轮询**裸机框架。目前仅支持任务轮询，后续会添加任务的挂起与就绪功能。

**时间片轮询**是通过一个基本时钟（**systick** or **timer**），针对不同的任务对 CPU 的处理时间进行时分复用，根据任务列表顺序执行。避免单任务长时间占用 CPU，有一定的实时性 ~~不是很实时~~。

---

## 使用说明

### 如何将**tp_framework_demo**添加入工程

- 将 tp_framework_demo.c/h 存放到工程路径下。

- 周期性调用 `TPF_Timebase_Int()`函数，如在**systick** or **timer**的中断中。

- 创建 `tp_frame_t` 实例作为框架句柄。

- 创建 `tp_tasklist_t` 实例作为任务表句柄，在表中填入任务函数及时间片。

- 调用`GET_TASK_NUMS_()`宏，统计任务表的任务量。

- 调用`TPF_Init()`函数，初始化框架句柄。

- 调用`TPF_Handler()`函数，进行任务轮询。

- **仔细**
  - 在进行时间片的分配操作前，应估算任务执行时间，过长的任务可能会**淹没**其他任务。
  - 请勿将时序相关操作分割在不同的任务中，**时序敏感**的操作尽量在框架之外运行。
  - 最小的时间片仅支持 1 个时基单位，确保任务轮询的调用**频繁于**时基更新。

### 任务表结构

- 表头固定为`{NULL, 0, 0}`,用作统计任务量。

- 通过创建、删除表成员来增、减任务，表成员的三个参数依次为：
  - 任务函数指针
  - 任务的时间片
  - 上次运行时间

```c
typedef struct
{
  void (*pff)();
  uint32_t runtick;
  uint32_t entrytick;
} tp_tasklist_t;
```

---

## 函数接口与演示

### 函数接口

```
func
  --- TPF_Timebase_Int()
  --- TPF_GetTicks()
    return
      --- newtick                   --- 最新时基数
  --- TPF_Init(tp_frame_t *,tp_tasklist_t *)
    params
      --- pstpfhandle               --- TPF句柄
      --- psatasklist               --- 任务表句柄
  --- TPF_Handler(tp_frame_t *)
    params
      --- pstpfhandle               --- TPF句柄
```

#### TPF_ErrorHandler

当程序进入 `TPF_ErrorHandler()`函数时，可通过查看 `pftasklist` 和 `error` 定位错误。

```
  error
   0  ------------ 检查任务表的指针
  -1  ------------ 检查TPF句柄是否有效
  -2  ------------ 检查任务表句柄是否有效
```

### 演示模板

```C
/*----------------------  it.c  ----------------------*/
void SysTick_Handler()//TIMx_IRQHandler()
{
    TPF_Timebase_Int();
}

/*---------------------  main.c  ---------------------*/
void Task0(void)
{
    static uint32_t t0_cnt;
    printf("T0:%d\r\n",t0_cnt++);
}
void Task1(void)
{
    static uint32_t t1_cnt;
    printf("T1:%d\r\n",t1_cnt++);
}

tp_frame_t tp_frame;
tp_tasklist_t tp_tasklist[] = {
      {NULL, 0, 0}, // dont change and use this var
//(taskfunc),(task_runtime),always 0
      {Task0, 1000, 0},
      {Task1, 200, 0}

};
int main(void)
{
  GET_TASK_NUMS_(tp_tasklist);
  TPF_Init(&tp_frame, tp_tasklist);

  while(1)
  {
    TPF_Handler(&tp_frame);
  }
}
```
