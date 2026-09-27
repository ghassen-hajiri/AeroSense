#include "scheduler.h"


#define SCHEDULER_PERIOD_10MS      10U
#define SCHEDULER_PERIOD_20MS      20U
#define SCHEDULER_PERIOD_100MS     100U
#define SCHEDULER_PERIOD_1000MS    1000U


static SCHEDULER_Tasks_t scheduler_tasks;

static uint32_t last_10ms;
static uint32_t last_20ms;
static uint32_t last_100ms;
static uint32_t last_1000ms;


void SCHEDULER_Init(uint32_t now_ms)
{
    scheduler_tasks.task_10ms_due = 0U;
    scheduler_tasks.task_20ms_due = 0U;
    scheduler_tasks.task_100ms_due = 0U;
    scheduler_tasks.task_1000ms_due = 0U;

    last_10ms = now_ms;
    last_20ms = now_ms;
    last_100ms = now_ms;
    last_1000ms = now_ms;
}


void SCHEDULER_Update(uint32_t now_ms)
{
    /*
     * Clear task flags at the beginning of each scheduler cycle.
     */
    scheduler_tasks.task_10ms_due = 0U;
    scheduler_tasks.task_20ms_due = 0U;
    scheduler_tasks.task_100ms_due = 0U;
    scheduler_tasks.task_1000ms_due = 0U;


    if ((uint32_t)(now_ms - last_10ms) >=
        SCHEDULER_PERIOD_10MS)
    {
        last_10ms = now_ms;
        scheduler_tasks.task_10ms_due = 1U;
    }


    if ((uint32_t)(now_ms - last_20ms) >=
        SCHEDULER_PERIOD_20MS)
    {
        last_20ms = now_ms;
        scheduler_tasks.task_20ms_due = 1U;
    }


    if ((uint32_t)(now_ms - last_100ms) >=
        SCHEDULER_PERIOD_100MS)
    {
        last_100ms = now_ms;
        scheduler_tasks.task_100ms_due = 1U;
    }


    if ((uint32_t)(now_ms - last_1000ms) >=
        SCHEDULER_PERIOD_1000MS)
    {
        last_1000ms = now_ms;
        scheduler_tasks.task_1000ms_due = 1U;
    }
}


const SCHEDULER_Tasks_t *SCHEDULER_GetTasks(void)
{
    return &scheduler_tasks;
}