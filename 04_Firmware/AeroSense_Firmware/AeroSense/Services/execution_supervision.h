#ifndef EXECUTION_SUPERVISION_H
#define EXECUTION_SUPERVISION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    EXECUTION_SUPERVISION_OK = 0,
    EXECUTION_SUPERVISION_ERROR

} EXECUTION_SUPERVISION_Status_t;


/*
 * Initializes execution supervision.
 */
void EXECUTION_SUPERVISION_Init(uint32_t now_ms);


/*
 * Reports successful execution of the required
 * cyclic task categories.
 */
void EXECUTION_SUPERVISION_Report10msTask(void);
void EXECUTION_SUPERVISION_Report20msTask(void);
void EXECUTION_SUPERVISION_Report100msTask(void);


/*
 * Evaluates the execution supervision window.
 *
 * The function shall be called cyclically.
 * Every 500 ms it verifies that all required
 * task categories executed at least once.
 *
 * The watchdog is refreshed only if supervision
 * is successful.
 */
EXECUTION_SUPERVISION_Status_t
EXECUTION_SUPERVISION_Update(uint32_t now_ms);


#ifdef __cplusplus
}
#endif

#endif /* EXECUTION_SUPERVISION_H */