#ifndef __TP_FRAMEWORK_H__
#define __TP_FRAMEWORK_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // 任务回调函数类型
    typedef void (*TaskFunc)(void);
    // 超时回调函数类型
    typedef void (*TimeoutCallback)(int task_id);
    // 任务状态
    typedef enum
    {
        TASK_READY,     // 任务就绪
        TASK_RUNNING,   // 任务正在执行
        TASK_SUSPENDED, // 任务被挂起
        TASK_TIMEOUT    // 任务执行超时
    } TaskState;

    /**
     * @brief 初始化调度器
     *
     * 初始化调度器内部状态，清空任务列表，必须在调用其他调度器函数前执行。
     */
    void Scheduler_Init(void);

    /**
     * @brief 添加任务到调度器
     *
     * @param func 任务函数指针，不能为NULL
     * @param interval 任务执行间隔(毫秒)，必须大于0
     * @param timeout 任务最大允许执行时间(毫秒)，0表示不检测超时
     * @return int 成功返回任务ID(>=0)，失败返回-1
     */
    int Scheduler_AddTask(TaskFunc func, uint32_t interval, uint32_t timeout);

    /**
     * @brief 删除指定任务
     *
     * @param task_id 要删除的任务ID
     * @return true 删除成功
     * @return false 删除失败(无效的任务ID)
     */
    bool Scheduler_DeleteTask(int task_id);

    /**
     * @brief 暂停指定任务
     *
     * @param task_id 要暂停的任务ID
     * @return true 暂停成功
     * @return false 暂停失败(无效的任务ID)
     */
    bool Scheduler_SuspendTask(int task_id);

    /**
     * @brief 恢复被暂停的任务
     *
     * @param task_id 要恢复的任务ID
     * @return true 恢复成功
     * @return false 恢复失败(无效的任务ID或任务未暂停)
     */
    bool Scheduler_ResumeTask(int task_id);

    /**
     * @brief 获取任务当前状态
     *
     * @param task_id 要查询的任务ID
     * @return TaskState 任务状态枚举值
     */
    TaskState Scheduler_GetTaskState(int task_id);

    /**
     * @brief 调度器主循环
     *
     * 需要在主程序中循环调用，负责执行满足条件的任务。
     * 注意：此函数会阻塞直到所有就绪任务执行完毕。
     */
    void Scheduler_Run(void);

    /**
     * @brief 系统时钟更新
     *
     * 需要在1ms定时器中断中调用，用于维护系统时间基准。
     */
    extern inline void Scheduler_TickUpdate(void);

    /**
     * @brief 设置任务超时回调函数
     *
     * @param callback 超时回调函数指针，当任务执行超时时调用
     */
    void Scheduler_SetTimeoutCallback(TimeoutCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* __TP_FRAMEWORK_H__ */