/**
 * ************************************************************************
 *
 * @file tp_framework.h
 * @author wsx
 * @brief
 *
 * ************************************************************************
 * @copyright Copyright (c) 2024 wsx
 * ************************************************************************
 */

#ifndef __TP_FRAMEWORK__
#define __TP_FRAMEWORK__

#include "stdint.h"
#include "stdlib.h"

#define GET_TASK_NUMS_(x) (x)[0].runtick = sizeof(x) / sizeof(tp_tasklist_t) - 1

// you should be create a global array of tasklist like this , dont ignore "{NULL,0,0}".
/*
************************************************************************
tp_tasklist_t tpf_task_list[] = {
        {NULL,0,0},  // dont change and use this var
//(taskfunc),(task_runtime),always 0
        {task0, 1000,0},
        {task1, 500,0},
        {task2, 2000,0},
//        ...
 };
************************************************************************
 */

typedef struct
{
  void (*pff)();
  uint32_t runtick;
  uint32_t entrytick;
} tp_tasklist_t;

typedef struct
{
  tp_tasklist_t(*pftasklist);
  uint32_t tasknum;
} tp_frame_t;

void TPF_Timebase_Int(void);
uint32_t TPF_GetTicks(void);

int8_t TPF_Init(tp_frame_t *pstpfhandle, tp_tasklist_t *psatasklist);
int8_t TPF_Handler(tp_frame_t *pstpfhandle); // run in your timer interrupt

#endif // !__TP_FRAMEWORK__