#ifndef WATCHDOG_H
#define WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    WATCHDOG_OK = 0,
    WATCHDOG_ERROR

} WATCHDOG_Status_t;


/*
 * Initializes the AeroSense watchdog driver.
 *
 * The STM32 IWDG peripheral itself is initialized by CubeMX
 * before this function is called.
 */
WATCHDOG_Status_t WATCHDOG_Init(void);


/*
 * Refreshes the independent watchdog.
 *
 * This function shall only be called after successful
 * execution supervision.
 */
WATCHDOG_Status_t WATCHDOG_Refresh(void);


#ifdef __cplusplus
}
#endif

#endif /* WATCHDOG_H */