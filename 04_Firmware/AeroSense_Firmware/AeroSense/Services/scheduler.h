#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    uint8_t task_10ms_due;
    uint8_t task_20ms_due;
    uint8_t task_100ms_due;
    uint8_t task_1000ms_due;

} SCHEDULER_Tasks_t;


/**
 * @brief Initializes scheduler timing references.
 *
 * @param now_ms Current system time in milliseconds.
 */
void SCHEDULER_Init(uint32_t now_ms);


/**
 * @brief Updates scheduler task flags.
 *
 * @param now_ms Current system time in milliseconds.
 */
void SCHEDULER_Update(uint32_t now_ms);


/**
 * @brief Returns the current scheduler task flags.
 */
const SCHEDULER_Tasks_t *SCHEDULER_GetTasks(void);


#ifdef __cplusplus
}
#endif

#endif /* SCHEDULER_H */