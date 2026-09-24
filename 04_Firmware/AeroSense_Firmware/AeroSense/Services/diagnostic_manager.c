#include "diagnostic_manager.h"

#include <stddef.h>


#define DIAGNOSTIC_FAULT_COUNT    10U


static DIAGNOSTIC_Fault_t diagnostic_faults[DIAGNOSTIC_FAULT_COUNT];


static int32_t DIAGNOSTIC_GetFaultIndex(
    DIAGNOSTIC_FaultId_t fault_id)
{
    if ((fault_id < DIAGNOSTIC_FAULT_TEMPERATURE) ||
        (fault_id > DIAGNOSTIC_FAULT_WATCHDOG_RESET))
    {
        return -1;
    }

    return (int32_t)fault_id - 1;
}


DIAGNOSTIC_Status_t DIAGNOSTIC_Init(void)
{
    uint32_t i;

    for (i = 0U; i < DIAGNOSTIC_FAULT_COUNT; i++)
    {
        diagnostic_faults[i].fault_id =
            (DIAGNOSTIC_FaultId_t)(i + 1U);

        diagnostic_faults[i].severity =
            DIAGNOSTIC_SEVERITY_INFO;

        diagnostic_faults[i].active = 0U;
    }

    return DIAGNOSTIC_OK;
}


DIAGNOSTIC_Status_t DIAGNOSTIC_SetFault(
    DIAGNOSTIC_FaultId_t fault_id,
    DIAGNOSTIC_Severity_t severity)
{
    int32_t index;

    index = DIAGNOSTIC_GetFaultIndex(fault_id);

    if (index < 0)
    {
        return DIAGNOSTIC_INVALID_ARG;
    }

    if (severity > DIAGNOSTIC_SEVERITY_CRITICAL)
    {
        return DIAGNOSTIC_INVALID_ARG;
    }

    diagnostic_faults[index].severity = severity;
    diagnostic_faults[index].active = 1U;

    return DIAGNOSTIC_OK;
}


DIAGNOSTIC_Status_t DIAGNOSTIC_ClearFault(
    DIAGNOSTIC_FaultId_t fault_id)
{
    int32_t index;

    index = DIAGNOSTIC_GetFaultIndex(fault_id);

    if (index < 0)
    {
        return DIAGNOSTIC_INVALID_ARG;
    }

    diagnostic_faults[index].active = 0U;
    diagnostic_faults[index].severity =
        DIAGNOSTIC_SEVERITY_INFO;

    return DIAGNOSTIC_OK;
}


uint8_t DIAGNOSTIC_IsFaultActive(
    DIAGNOSTIC_FaultId_t fault_id)
{
    int32_t index;

    index = DIAGNOSTIC_GetFaultIndex(fault_id);

    if (index < 0)
    {
        return 0U;
    }

    return diagnostic_faults[index].active;
}


DIAGNOSTIC_Severity_t DIAGNOSTIC_GetHighestSeverity(void)
{
    DIAGNOSTIC_Severity_t highest =
        DIAGNOSTIC_SEVERITY_INFO;

    uint32_t i;

    for (i = 0U; i < DIAGNOSTIC_FAULT_COUNT; i++)
    {
        if ((diagnostic_faults[i].active != 0U) &&
            (diagnostic_faults[i].severity > highest))
        {
            highest = diagnostic_faults[i].severity;
        }
    }

    return highest;
}


uint16_t DIAGNOSTIC_GetActiveFaultCount(void)
{
    uint16_t count = 0U;
    uint32_t i;

    for (i = 0U; i < DIAGNOSTIC_FAULT_COUNT; i++)
    {
        if (diagnostic_faults[i].active != 0U)
        {
            count++;
        }
    }

    return count;
}


DIAGNOSTIC_FaultId_t DIAGNOSTIC_GetPrimaryFault(void)
{
    DIAGNOSTIC_FaultId_t primary =
        DIAGNOSTIC_FAULT_NONE;

    DIAGNOSTIC_Severity_t highest =
        DIAGNOSTIC_SEVERITY_INFO;

    uint32_t i;

    for (i = 0U; i < DIAGNOSTIC_FAULT_COUNT; i++)
    {
        if (diagnostic_faults[i].active != 0U)
        {
            if ((primary == DIAGNOSTIC_FAULT_NONE) ||
                (diagnostic_faults[i].severity > highest))
            {
                primary = diagnostic_faults[i].fault_id;
                highest = diagnostic_faults[i].severity;
            }
        }
    }

    return primary;
}