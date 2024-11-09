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

#include "tp_framework.h"
#include "stdlib.h"
#include <assert.h>


#define __COUNT_CURR_OFFSET (uint32_t)pstpfhandle \
                            - (uint32_t)pstpfhandle->pftasklist
#define __COUNT_TASK_OFFSET (tpf_to_list_offset - curr_address_offset) \
                            / sizeof(tpf_tasklist_t)
                                
static volatile uint32_t newtick;
// record the address offset , dont change it after init.
static uint32_t tpf_to_list_offset;

__attribute__((weak)) void TPF_ErrorHandler(tpf_tasklist_t *pstasklist,
                                                          int8_t error)
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
    for(uint32_t i = 0; i < pstpfhandle->tasknum; i++)
    {
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
        if(i == pstpfhandle->tasknum - 1)
            pstpfhandle->pftasklist -= pstpfhandle->tasknum;
    }
    
    return 0;
}

int8_t TPF_Init(tp_frame_t *pstpfhandle,
            tpf_tasklist_t *psatasklist,
                        uint8_t tasknum)
{
    if (NULL == pstpfhandle)
    {
        TPF_ErrorHandler(NULL, -1);
    }
    if (NULL == psatasklist)
    {
        TPF_ErrorHandler(NULL, -2);
    }

    pstpfhandle->tasknum = tasknum;
    pstpfhandle->pftasklist = psatasklist;
    tpf_to_list_offset = (uint32_t)pstpfhandle - (uint32_t)psatasklist;

    return 0;
}

void TPF_Task_Delay(tp_frame_t *pstpfhandle,
                            uint32_t task_id,
                            uint32_t delay)
{
    assert(task_id<pstpfhandle->tasknum);
    
    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    // get now task offset of tpf
    uint32_t curr_address_offset = __COUNT_CURR_OFFSET;
    uint32_t task_offset = __COUNT_TASK_OFFSET;

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
}

int8_t TPF_Suspend(tp_frame_t *pstpfhandle, uint32_t task_id)
{
    assert(task_id<pstpfhandle->tasknum);

    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }
    
    // get now task offset of tpf
    uint32_t curr_address_offset = __COUNT_CURR_OFFSET;
    uint32_t task_offset = __COUNT_TASK_OFFSET;

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

    return 0;
}

int8_t TPF_Resume(tp_frame_t *pstpfhandle, uint32_t task_id)
{
    assert(task_id<pstpfhandle->tasknum);
    
    if (NULL == pstpfhandle->pftasklist->pff)
    {
        TPF_ErrorHandler(pstpfhandle->pftasklist, 0);
    }

    // get now task offset of tpf
    uint32_t curr_address_offset = __COUNT_CURR_OFFSET;
    uint32_t task_offset = __COUNT_TASK_OFFSET;

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

    return 0;
}
