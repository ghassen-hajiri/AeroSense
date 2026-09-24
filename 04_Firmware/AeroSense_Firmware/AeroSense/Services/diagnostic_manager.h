#ifndef DIAGNOSTIC_MANAGER_H
#define DIAGNOSTIC_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DIAGNOSTIC_OK = 0,
    DIAGNOSTIC_ERROR,
    DIAGNOSTIC_INVALID_ARG
} DIAGNOSTIC_Status_t;

typedef enum
{
    DIAGNOSTIC_SEVERITY_INFO = 0,
    DIAGNOSTIC_SEVERITY_DEGRADED,
    DIAGNOSTIC_SEVERITY_CRITICAL
} DIAGNOSTIC_Severity_t;

typedef enum
{
    DIAGNOSTIC_FAULT_NONE              = 0x0000U,

    DIAGNOSTIC_FAULT_TEMPERATURE       = 0x0001U,
    DIAGNOSTIC_FAULT_PRESSURE          = 0x0002U,
    DIAGNOSTIC_FAULT_ACCELERATION      = 0x0003U,
    DIAGNOSTIC_FAULT_VIN_ADC           = 0x0004U,
    DIAGNOSTIC_FAULT_VIN_UNDERVOLTAGE  = 0x0005U,
    DIAGNOSTIC_FAULT_CAN               = 0x0006U,
    DIAGNOSTIC_FAULT_USB               = 0x0007U,
    DIAGNOSTIC_FAULT_INITIALIZATION    = 0x0008U,
    DIAGNOSTIC_FAULT_EXECUTION         = 0x0009U,
    DIAGNOSTIC_FAULT_WATCHDOG_RESET    = 0x000AU
} DIAGNOSTIC_FaultId_t;

typedef struct
{
    DIAGNOSTIC_FaultId_t fault_id;
    DIAGNOSTIC_Severity_t severity;
    uint8_t active;
} DIAGNOSTIC_Fault_t;


DIAGNOSTIC_Status_t DIAGNOSTIC_Init(void);

DIAGNOSTIC_Status_t DIAGNOSTIC_SetFault(
    DIAGNOSTIC_FaultId_t fault_id,
    DIAGNOSTIC_Severity_t severity);

DIAGNOSTIC_Status_t DIAGNOSTIC_ClearFault(
    DIAGNOSTIC_FaultId_t fault_id);

uint8_t DIAGNOSTIC_IsFaultActive(
    DIAGNOSTIC_FaultId_t fault_id);

DIAGNOSTIC_Severity_t DIAGNOSTIC_GetHighestSeverity(void);

uint16_t DIAGNOSTIC_GetActiveFaultCount(void);

DIAGNOSTIC_FaultId_t DIAGNOSTIC_GetPrimaryFault(void);


#ifdef __cplusplus
}
#endif

#endif /* DIAGNOSTIC_MANAGER_H */