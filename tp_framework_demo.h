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

#ifndef __TP_FRAMEWORK__
#define __TP_FRAMEWORK__

#include "stdint.h"
#include "stdlib.h"

#define GET_TASK_NUMS_(x) (x)[0].runtick = sizeof(x) / sizeof(tpf_tasklist_t) - 1

// you should be create a global array of tasklist like this , dont ignore "{NULL,0,0,0}".
/*
************************************************************************
tpf_tasklist_t tpf_tasklist[] = {
      {NULL, 0, 0, 0}, // dont change and use this var
//(taskfunc),(task_runtime),task_state,always 0
      {Task0, 1000, TP_TASK_READY, 0},
      {Task1, 200, TP_TASK_READY, 0}
//      ...
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

int8_t TPF_Init(tp_frame_t *pstpfhandle, tpf_tasklist_t *psatasklist);
int8_t TPF_Handler(tp_frame_t *pstpfhandle); // run in your timer interrupt

void TPF_Task_Delay(tp_frame_t *pstpfhandle, uint32_t task_id, uint32_t delay);
void TPF_Global_Delay(tp_frame_t *pstpfhandle, uint32_t delay);
int8_t TPF_Suspend(tp_frame_t *pstpfhandle, uint32_t task_id);
int8_t TPF_Resume(tp_frame_t *pstpfhandle, uint32_t task_id);

#endif // !__TP_FRAMEWORK__
