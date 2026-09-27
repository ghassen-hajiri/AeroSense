#ifndef MEASUREMENT_MANAGER_H
#define MEASUREMENT_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    MEASUREMENT_MANAGER_OK = 0,
    MEASUREMENT_MANAGER_ERROR
} MEASUREMENT_MANAGER_Status_t;


typedef struct
{
    float temperature_c;
    float pressure_hpa;

    float acceleration_x_g;
    float acceleration_y_g;
    float acceleration_z_g;

    float vin_voltage;

    uint8_t temperature_valid;
    uint8_t pressure_valid;
    uint8_t acceleration_valid;
    uint8_t vin_valid;

} MEASUREMENT_MANAGER_Data_t;


/**
 * @brief Initializes all measurement sources.
 *
 * @return MEASUREMENT_MANAGER_OK if all sources initialize successfully.
 */
MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_Init(void);


/**
 * @brief Updates the temperature measurement.
 */
MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_UpdateTemperature(void);


/**
 * @brief Updates the pressure measurement.
 */
MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_UpdatePressure(void);


/**
 * @brief Updates the acceleration measurement.
 */
MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_UpdateAcceleration(void);


/**
 * @brief Updates the VIN measurement.
 */
MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_UpdateVIN(void);


/**
 * @brief Attempts recovery of measurement channels
 *        with an active diagnostic fault.
 *
 * Recovery reinitializes the affected device.
 * The diagnostic fault remains active until the
 * normal three-success confirmation logic clears it.
 *
 * @return MEASUREMENT_MANAGER_OK if all required
 *         recovery attempts were successful.
 */
MEASUREMENT_MANAGER_Status_t
MEASUREMENT_MANAGER_ProcessRecovery(void);


/**
 * @brief Returns the latest measurement data.
 *
 * @return Pointer to the latest measurement data.
 */
const MEASUREMENT_MANAGER_Data_t *MEASUREMENT_MANAGER_GetData(void);


#ifdef __cplusplus
}
#endif

#endif /* MEASUREMENT_MANAGER_H */