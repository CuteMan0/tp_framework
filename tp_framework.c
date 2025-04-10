#include "tp_framework.h"

// 任务控制块结构
typedef struct
{
    TaskFunc func;       // 任务函数指针
    uint32_t interval;   // 执行间隔
    uint32_t last_run;   // 上次执行时间
    uint32_t timeout;    // 超时时间
    uint32_t start_time; // 任务开始执行时间
    TaskState state;     // 任务状态
    bool valid;          // 任务槽位是否有效
} TaskControlBlock;

#define MAX_TASKS 10       // 最大任务数量
#define INVALID_TASK_ID -1 // 无效任务ID

static TaskControlBlock task_list[MAX_TASKS];
static volatile uint32_t system_tick = 0;
static TimeoutCallback timeout_cb = NULL;

static int find_free_slot(void)
{
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (!task_list[i].valid)
        {
            return i;
        }
    }
    return INVALID_TASK_ID;
}

// 检查任务ID是否有效
static bool is_valid_task_id(int task_id)
{
    return (task_id >= 0 && task_id < MAX_TASKS && task_list[task_id].valid);
}

void Scheduler_Init(void)
{
    system_tick = 0;
    timeout_cb = NULL;

    for (int i = 0; i < MAX_TASKS; i++)
    {
        task_list[i].valid = false;
        task_list[i].state = TASK_SUSPENDED;
    }
}

int Scheduler_AddTask(TaskFunc func, uint32_t interval, uint32_t timeout)
{
    if (func == NULL || interval == 0)
    {
        return INVALID_TASK_ID;
    }

    int task_id = find_free_slot();
    if (task_id == INVALID_TASK_ID)
    {
        return INVALID_TASK_ID;
    }

    task_list[task_id].func = func;
    task_list[task_id].interval = interval;
    task_list[task_id].last_run = system_tick;
    task_list[task_id].timeout = timeout;
    task_list[task_id].state = TASK_READY;
    task_list[task_id].valid = true;

    return task_id;
}

bool Scheduler_DeleteTask(int task_id)
{
    if (!is_valid_task_id(task_id))
    {
        return false;
    }

    task_list[task_id].valid = false;
    return true;
}

bool Scheduler_SuspendTask(int task_id)
{
    if (!is_valid_task_id(task_id))
    {
        return false;
    }

    task_list[task_id].state = TASK_SUSPENDED;
    return true;
}

bool Scheduler_ResumeTask(int task_id)
{
    if (!is_valid_task_id(task_id))
    {
        return false;
    }

    task_list[task_id].state = TASK_READY;
    task_list[task_id].last_run = system_tick; // 重置计时
    return true;
}

TaskState Scheduler_GetTaskState(int task_id)
{
    if (!is_valid_task_id(task_id))
    {
        return TASK_SUSPENDED;
    }
    return task_list[task_id].state;
}

void Scheduler_Run(void)
{
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (!task_list[i].valid || task_list[i].state != TASK_READY)
        {
            continue;
        }

        // 检查是否到达执行时间
        if ((system_tick - task_list[i].last_run) >= task_list[i].interval)
        {
            task_list[i].state = TASK_RUNNING;
            task_list[i].start_time = system_tick;
            task_list[i].last_run = system_tick;

            // 执行任务
            task_list[i].func();

            // 检查是否超时
            if (task_list[i].timeout > 0 &&
                (system_tick - task_list[i].start_time) >= task_list[i].timeout)
            {
                task_list[i].state = TASK_TIMEOUT;
                if (timeout_cb != NULL)
                {
                    timeout_cb(i);
                }
            }
            else
            {
                task_list[i].state = TASK_READY;
            }
        }
    }
}

inline void Scheduler_TickUpdate(void)
{
    system_tick++;
}

void Scheduler_SetTimeoutCallback(TimeoutCallback callback)
{
    timeout_cb = callback;
}