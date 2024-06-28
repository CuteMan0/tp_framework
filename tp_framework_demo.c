/**
 * ************************************************************************
 *
 * @file tp_framework.c
 * @author wsx
 * @brief
 *
 * ************************************************************************
 * @copyright Copyright (c) 2024 lovely-lily
 * ************************************************************************
 */

#include "tp_framework_demo.h"

static uint32_t newtick;

__attribute__((weak)) void TPF_ErrorHandler(tpf_tasklist_t *pftasklist, int8_t error)
{
    while (1)
    {
        // please check any pointer of tp_framework
        /*
        error = 0   ------------    check task function pointer
                -1  ------------    check tpf handle pointer        (TPF_Init)
                -2  ------------    check tasklist pointer          (TPF_Init)

        */
    }
}

// run in your timer interrupt
void TPF_Timebase_Int(void)
{
    newtick++;
}

uint32_t TPF_GetTicks(void)
{
    return newtick;
}

int8_t TPF_Handler(tp_frame_t *pstpfhandle)
{
    static uint32_t i = 0;
    if (i < pstpfhandle->tasknum)
    {
        i++;
        // check task func pointer
        if (NULL == pstpfhandle->pftasklist->pff)
        {
            TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
        }

        if (newtick >= pstpfhandle->pftasklist->entrytick)
        {
            // update entry tick
            pstpfhandle->pftasklist->entrytick += pstpfhandle->pftasklist->runtick;

            if (TPF_TASK_SLEEP != pstpfhandle->pftasklist->state)
            {
                // task running
                pstpfhandle->pftasklist->pff();
            }
        }

        pstpfhandle->pftasklist++;

        if (i == pstpfhandle->tasknum)
        {
            pstpfhandle->pftasklist -= i;
            i = 0;
        }
    }

    return 0;
}

int8_t TPF_Init(tp_frame_t *pstpfhandle, tpf_tasklist_t *psatasklist)
{
    if (NULL == pstpfhandle)
    {
        TPF_ErrorHandler(NULL, -1);
    }
    if (NULL == psatasklist)
    {
        TPF_ErrorHandler(NULL, -2);
    }

    pstpfhandle->tasknum = psatasklist[0].runtick;
    pstpfhandle->pftasklist = ++psatasklist;

    return 0;
}

void TPF_Task_Delay(tp_frame_t *pstpfhandle, uint32_t task_id, uint32_t delay)
{
    uint32_t address_offset = ((uint32_t)pstpfhandle - (uint32_t)pstpfhandle->pftasklist) / sizeof(tpf_tasklist_t);
    uint32_t task_offset = pstpfhandle->tasknum - address_offset; // little endian

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    // change target task
    pstpfhandle->pftasklist -= task_offset;
    pstpfhandle->pftasklist += task_id;

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    pstpfhandle->pftasklist->entrytick += delay;

    // restore enter task
    pstpfhandle->pftasklist -= task_id;
    pstpfhandle->pftasklist += task_offset;

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }
}

void TPF_Global_Delay(tp_frame_t *pstpfhandle, uint32_t delay)
{

    uint32_t address_offset = ((uint32_t)pstpfhandle - (uint32_t)pstpfhandle->pftasklist) / sizeof(tpf_tasklist_t);
    uint32_t task_offset = pstpfhandle->tasknum - address_offset; // little endian

    for (uint32_t i = 0; i < address_offset; i++)
    {
        if (NULL == pstpfhandle->pftasklist->pff)
        {
            TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
        }

        pstpfhandle->pftasklist->entrytick += delay;
        pstpfhandle->pftasklist++;
    }
    pstpfhandle->pftasklist -= pstpfhandle->tasknum;

    for (uint32_t i = 0; i < task_offset; i++)
    {
        if (NULL == pstpfhandle->pftasklist->pff)
        {
            TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
        }

        pstpfhandle->pftasklist->entrytick += delay;
        pstpfhandle->pftasklist++;
    }

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }
}

int8_t TPF_Suspend(tp_frame_t *pstpfhandle, uint32_t task_id)
{
    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    uint32_t address_offset = ((uint32_t)pstpfhandle - (uint32_t)pstpfhandle->pftasklist) / sizeof(tpf_tasklist_t);
    uint32_t task_offset = pstpfhandle->tasknum - address_offset; // little endian

    // change target task
    pstpfhandle->pftasklist -= task_offset;
    pstpfhandle->pftasklist += task_id;

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    if (TPF_TASK_SLEEP != pstpfhandle->pftasklist->state)
    {
        pstpfhandle->pftasklist->state = TPF_TASK_SLEEP;
    }

    // restore enter task
    pstpfhandle->pftasklist -= task_id;
    pstpfhandle->pftasklist += task_offset;

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }
}

int8_t TPF_Resume(tp_frame_t *pstpfhandle, uint32_t task_id)
{
    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    uint32_t address_offset = ((uint32_t)pstpfhandle - (uint32_t)pstpfhandle->pftasklist) / sizeof(tpf_tasklist_t);
    uint32_t task_offset = pstpfhandle->tasknum - address_offset; // little endian

    // change target task
    pstpfhandle->pftasklist -= task_offset;
    pstpfhandle->pftasklist += task_id;

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    if (TPF_TASK_READY != pstpfhandle->pftasklist->state)
    {
        pstpfhandle->pftasklist->state = TPF_TASK_READY;
    }

    // restore enter task
    pstpfhandle->pftasklist -= task_id;
    pstpfhandle->pftasklist += task_offset;

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }
}
