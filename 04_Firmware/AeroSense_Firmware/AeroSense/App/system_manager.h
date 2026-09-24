#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    SYSTEM_STATE_INITIALIZATION = 0,
    SYSTEM_STATE_SELF_TEST,
    SYSTEM_STATE_NORMAL_OPERATION,
    SYSTEM_STATE_DEGRADED_OPERATION,
    SYSTEM_STATE_FAULT
} SYSTEM_MANAGER_State_t;


typedef enum
{
    SYSTEM_MANAGER_OK = 0,
    SYSTEM_MANAGER_ERROR
} SYSTEM_MANAGER_Status_t;


SYSTEM_MANAGER_Status_t SYSTEM_MANAGER_Init(void);

SYSTEM_MANAGER_Status_t SYSTEM_MANAGER_Update(void);

SYSTEM_MANAGER_State_t SYSTEM_MANAGER_GetState(void);


#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_MANAGER_H */