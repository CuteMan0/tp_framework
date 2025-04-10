#ifndef __TP_FRAMEWORK_H__
#define __TP_FRAMEWORK_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // ����ص���������
    typedef void (*TaskFunc)(void);
    // ��ʱ�ص���������
    typedef void (*TimeoutCallback)(int task_id);
    // ����״̬
    typedef enum
    {
        TASK_READY,     // �������
        TASK_RUNNING,   // ��������ִ��
        TASK_SUSPENDED, // ���񱻹���
        TASK_TIMEOUT    // ����ִ�г�ʱ
    } TaskState;

    /**
     * @brief ��ʼ��������
     *
     * ��ʼ���������ڲ�״̬����������б������ڵ�����������������ǰִ�С�
     */
    void Scheduler_Init(void);

    /**
     * @brief ������񵽵�����
     *
     * @param func ������ָ�룬����ΪNULL
     * @param interval ����ִ�м��(����)���������0
     * @param timeout �����������ִ��ʱ��(����)��0��ʾ����ⳬʱ
     * @return int �ɹ���������ID(>=0)��ʧ�ܷ���-1
     */
    int Scheduler_AddTask(TaskFunc func, uint32_t interval, uint32_t timeout);

    /**
     * @brief ɾ��ָ������
     *
     * @param task_id Ҫɾ��������ID
     * @return true ɾ���ɹ�
     * @return false ɾ��ʧ��(��Ч������ID)
     */
    bool Scheduler_DeleteTask(int task_id);

    /**
     * @brief ��ָͣ������
     *
     * @param task_id Ҫ��ͣ������ID
     * @return true ��ͣ�ɹ�
     * @return false ��ͣʧ��(��Ч������ID)
     */
    bool Scheduler_SuspendTask(int task_id);

    /**
     * @brief �ָ�����ͣ������
     *
     * @param task_id Ҫ�ָ�������ID
     * @return true �ָ��ɹ�
     * @return false �ָ�ʧ��(��Ч������ID������δ��ͣ)
     */
    bool Scheduler_ResumeTask(int task_id);

    /**
     * @brief ��ȡ����ǰ״̬
     *
     * @param task_id Ҫ��ѯ������ID
     * @return TaskState ����״̬ö��ֵ
     */
    TaskState Scheduler_GetTaskState(int task_id);

    /**
     * @brief ��������ѭ��
     *
     * ��Ҫ����������ѭ�����ã�����ִ����������������
     * ע�⣺�˺���������ֱ�����о�������ִ����ϡ�
     */
    void Scheduler_Run(void);

    /**
     * @brief ϵͳʱ�Ӹ���
     *
     * ��Ҫ��1ms��ʱ���ж��е��ã�����ά��ϵͳʱ���׼��
     */
    extern inline void Scheduler_TickUpdate(void);

    /**
     * @brief ��������ʱ�ص�����
     *
     * @param callback ��ʱ�ص�����ָ�룬������ִ�г�ʱʱ����
     */
    void Scheduler_SetTimeoutCallback(TimeoutCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* __TP_FRAMEWORK_H__ */