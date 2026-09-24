#include "system_manager.h"

#include "diagnostic_manager.h"


static SYSTEM_MANAGER_State_t system_state =
    SYSTEM_STATE_INITIALIZATION;


SYSTEM_MANAGER_Status_t SYSTEM_MANAGER_Init(void)
{
    system_state = SYSTEM_STATE_INITIALIZATION;

    return SYSTEM_MANAGER_OK;
}


SYSTEM_MANAGER_Status_t SYSTEM_MANAGER_Update(void)
{
    DIAGNOSTIC_Severity_t highest_severity;

    highest_severity = DIAGNOSTIC_GetHighestSeverity();


    switch (system_state)
    {
        case SYSTEM_STATE_INITIALIZATION:

            /*
             * Low-level peripheral initialization is performed
             * before the System Manager is started.
             *
             * After software initialization, continue with
             * the system self-test.
             */
            system_state = SYSTEM_STATE_SELF_TEST;

            break;


        case SYSTEM_STATE_SELF_TEST:

            if (highest_severity == DIAGNOSTIC_SEVERITY_CRITICAL)
            {
                system_state = SYSTEM_STATE_FAULT;
            }
            else if (DIAGNOSTIC_GetActiveFaultCount() > 0U)
            {
                system_state = SYSTEM_STATE_DEGRADED_OPERATION;
            }
            else
            {
                system_state = SYSTEM_STATE_NORMAL_OPERATION;
            }

            break;


        case SYSTEM_STATE_NORMAL_OPERATION:

            if (highest_severity == DIAGNOSTIC_SEVERITY_CRITICAL)
            {
                system_state = SYSTEM_STATE_FAULT;
            }
            else if (DIAGNOSTIC_GetActiveFaultCount() > 0U)
            {
                system_state = SYSTEM_STATE_DEGRADED_OPERATION;
            }
            else
            {
                /* Remain in NORMAL_OPERATION */
            }

            break;


        case SYSTEM_STATE_DEGRADED_OPERATION:

            if (highest_severity == DIAGNOSTIC_SEVERITY_CRITICAL)
            {
                system_state = SYSTEM_STATE_FAULT;
            }
            else if (DIAGNOSTIC_GetActiveFaultCount() == 0U)
            {
                system_state = SYSTEM_STATE_NORMAL_OPERATION;
            }
            else
            {
                /* Remain in DEGRADED_OPERATION */
            }

            break;


        case SYSTEM_STATE_FAULT:

            /*
             * FAULT is intentionally latched.
             *
             * Recovery from a critical fault requires a
             * controlled restart/reset in Revision A.
             */
            break;


        default:

            system_state = SYSTEM_STATE_FAULT;

            DIAGNOSTIC_SetFault(
                DIAGNOSTIC_FAULT_EXECUTION,
                DIAGNOSTIC_SEVERITY_CRITICAL);

            return SYSTEM_MANAGER_ERROR;
    }


    return SYSTEM_MANAGER_OK;
}


SYSTEM_MANAGER_State_t SYSTEM_MANAGER_GetState(void)
{
    return system_state;
}