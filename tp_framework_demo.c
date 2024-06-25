/**
 * ************************************************************************
 *
 * @file tp_framework.c
 * @author wsx
 * @brief
 *
 * ************************************************************************
 * @copyright Copyright (c) 2024 wsx
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
    for (uint8_t i = 0; i < pstpfhandle->tasknum; pstpfhandle->pftasklist++)
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
    }
    pstpfhandle->pftasklist -= pstpfhandle->tasknum;

    return 0;
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
