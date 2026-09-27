#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CONFIGURATION_MANAGER_OK = 0,
    CONFIGURATION_MANAGER_ERROR
} CONFIGURATION_MANAGER_Status_t;

/**
 * @brief Initializes the configuration manager.
 */
CONFIGURATION_MANAGER_Status_t CONFIGURATION_MANAGER_Init(void);

/**
 * @brief Processes received USB configuration commands.
 *
 * Call cyclically from the main scheduler.
 */
void CONFIGURATION_MANAGER_Update(void);

/**
 * @brief Returns the configured VIN undervoltage threshold.
 *
 * @return VIN undervoltage threshold in volts.
 */
float CONFIGURATION_MANAGER_GetVinUndervoltageThreshold(void);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_MANAGER_H */