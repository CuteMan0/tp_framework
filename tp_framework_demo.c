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
#include "stm32f1xx_hal.h"

static uint32_t newtick;

__attribute__((weak)) void TPF_ErrorHandler(tp_tasklist_t *pftasklist, int8_t error)
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
            // task running
            pstpfhandle->pftasklist->pff();
        }

        pstpfhandle->pftasklist++;

        if (i == pstpfhandle->tasknum)
        {
            pstpfhandle->pftasklist -= i;
            i = 0;
        }

        return 0;
    }
}

int8_t TPF_Init(tp_frame_t *pstpfhandle, tp_tasklist_t *psatasklist)
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
    uint32_t address_offset = ((uint32_t)pstpfhandle - (uint32_t)pstpfhandle->pftasklist) / sizeof(tp_tasklist_t);
    uint32_t task_offset = pstpfhandle->tasknum - address_offset; // little endian

    // change target task entrytick
    pstpfhandle->pftasklist -= task_offset;
    pstpfhandle->pftasklist += task_id;
    pstpfhandle->pftasklist->entrytick += delay;

    // restore enter task
    pstpfhandle->pftasklist -= task_id;
    pstpfhandle->pftasklist += task_offset;
}

void TPF_Global_Delay(tp_frame_t *pstpfhandle, uint32_t delay)
{
    uint32_t address_offset = ((uint32_t)pstpfhandle - (uint32_t)pstpfhandle->pftasklist) / sizeof(tp_tasklist_t);
    uint32_t task_offset = pstpfhandle->tasknum - address_offset; // little endian

    for (uint32_t i = 0; i < address_offset; i++)
    {
        pstpfhandle->pftasklist->entrytick += delay;
        pstpfhandle->pftasklist++;
    }
    pstpfhandle->pftasklist -= pstpfhandle->tasknum;

    for (uint32_t i = 0; i < task_offset; i++)
    {
        pstpfhandle->pftasklist->entrytick += delay;
        pstpfhandle->pftasklist++;
    }
}

/*****************************test functions start*********************************/
int8_t TPF_Suspend(tp_frame_t *pstpfhandle, uint32_t timeout)
{
}
int8_t TPF_Resume(tp_frame_t *pstpfhandle)
{
}

/***************************** test functions end *********************************/
