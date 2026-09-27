#include "execution_supervision.h"

#include "watchdog.h"
#include "diagnostic_manager.h"


#define EXECUTION_SUPERVISION_WINDOW_MS    500U


static uint32_t supervision_window_start_ms;

static uint8_t task_10ms_executed;
static uint8_t task_20ms_executed;
static uint8_t task_100ms_executed;


void EXECUTION_SUPERVISION_Init(uint32_t now_ms)
{
    supervision_window_start_ms = now_ms;

    task_10ms_executed = 0U;
    task_20ms_executed = 0U;
    task_100ms_executed = 0U;

    WATCHDOG_Init();
}


void EXECUTION_SUPERVISION_Report10msTask(void)
{
    task_10ms_executed = 1U;
}


void EXECUTION_SUPERVISION_Report20msTask(void)
{
    task_20ms_executed = 1U;
}


void EXECUTION_SUPERVISION_Report100msTask(void)
{
    task_100ms_executed = 1U;
}


EXECUTION_SUPERVISION_Status_t
EXECUTION_SUPERVISION_Update(uint32_t now_ms)
{
    if ((uint32_t)(now_ms - supervision_window_start_ms) <
        EXECUTION_SUPERVISION_WINDOW_MS)
    {
        return EXECUTION_SUPERVISION_OK;
    }


    supervision_window_start_ms = now_ms;


    if ((task_10ms_executed != 0U) &&
        (task_20ms_executed != 0U) &&
        (task_100ms_executed != 0U))
    {
        task_10ms_executed = 0U;
        task_20ms_executed = 0U;
        task_100ms_executed = 0U;

        DIAGNOSTIC_ClearFault(
            DIAGNOSTIC_FAULT_EXECUTION);

        if (WATCHDOG_Refresh() != WATCHDOG_OK)
        {
            DIAGNOSTIC_SetFault(
                DIAGNOSTIC_FAULT_EXECUTION,
                DIAGNOSTIC_SEVERITY_CRITICAL);

            return EXECUTION_SUPERVISION_ERROR;
        }

        return EXECUTION_SUPERVISION_OK;
    }


    /*
     * At least one required cyclic task category
     * did not execute within the supervision window.
     *
     * Do not refresh the watchdog.
     */
    DIAGNOSTIC_SetFault(
        DIAGNOSTIC_FAULT_EXECUTION,
        DIAGNOSTIC_SEVERITY_CRITICAL);


    task_10ms_executed = 0U;
    task_20ms_executed = 0U;
    task_100ms_executed = 0U;


    return EXECUTION_SUPERVISION_ERROR;
}