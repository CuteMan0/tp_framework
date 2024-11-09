/**
 * ************************************************************************
 *
 * @file tp_framework.h
 * @author wsx
 * @brief
 *
 * ************************************************************************
 * @copyright Copyright (c) 2024 lovely-lily
 * ************************************************************************
 */

#ifndef __TP_FRAMEWORK
#define __TP_FRAMEWORK

#include "stdint.h"


#define __GET_TASK_NUMS(x) sizeof(x) / sizeof(tpf_tasklist_t)

// you should be create a global array of tasklist like this
/*
************************************************************************
tpf_tasklist_t tpf_tasklist[] = {
//(taskfunc),(task_runtime),  (task_state)  ,(task_start_tick)
  {Task0    ,     1000     ,  TPF_TASK_READY,        0       }
 ,{Task1    ,     200      ,  TPF_TASK_READY,        0       }
//...
};
************************************************************************
 */

typedef enum
{
    TPF_TASK_READY,
    TPF_TASK_SLEEP
} TPF_STATE_E;

typedef struct
{
    void (*pff)();
    uint32_t runtick : 31;
    TPF_STATE_E state : 1;
    uint32_t entrytick;
} tpf_tasklist_t;

typedef struct
{
    tpf_tasklist_t(*pftasklist);
    int32_t tasknum;
} tp_frame_t;

void TPF_Timebase_Int(void);
uint32_t TPF_GetTicks(void);

int8_t TPF_Init(tp_frame_t *pstpfhandle, tpf_tasklist_t *psatasklist,uint8_t tasknum);
int8_t TPF_Handler(tp_frame_t *pstpfhandle); // run in your timer interrupt

void TPF_Task_Delay(tp_frame_t *pstpfhandle, uint32_t task_id, uint32_t delay);
int8_t TPF_Suspend(tp_frame_t *pstpfhandle, uint32_t task_id);
int8_t TPF_Resume(tp_frame_t *pstpfhandle, uint32_t task_id);

#endif // !__TP_FRAMEWORK
