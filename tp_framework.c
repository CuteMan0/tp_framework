#include "tp_framework.h"

// ������ƿ�ṹ
typedef struct
{
    TaskFunc func;       // ������ָ��
    uint32_t interval;   // ִ�м��
    uint32_t last_run;   // �ϴ�ִ��ʱ��
    uint32_t timeout;    // ��ʱʱ��
    uint32_t start_time; // ����ʼִ��ʱ��
    TaskState state;     // ����״̬
    bool valid;          // �����λ�Ƿ���Ч
} TaskControlBlock;

#define MAX_TASKS 10       // �����������
#define INVALID_TASK_ID -1 // ��Ч����ID

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

// �������ID�Ƿ���Ч
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
    task_list[task_id].last_run = system_tick; // ���ü�ʱ
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

        // ����Ƿ񵽴�ִ��ʱ��
        if ((system_tick - task_list[i].last_run) >= task_list[i].interval)
        {
            task_list[i].state = TASK_RUNNING;
            task_list[i].start_time = system_tick;
            task_list[i].last_run = system_tick;

            // ִ������
            task_list[i].func();

            // ����Ƿ�ʱ
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