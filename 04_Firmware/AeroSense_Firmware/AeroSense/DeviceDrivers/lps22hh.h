#ifndef LPS22HH_H
#define LPS22HH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
    LPS22HH_OK = 0,
    LPS22HH_ERROR,
    LPS22HH_DEVICE_NOT_FOUND,
    LPS22HH_INVALID_ARG
} LPS22HH_Status_t;

/**
 * @brief Initializes the LPS22HH pressure sensor.
 *
 * Checks the device identity and configures the sensor
 * for continuous pressure measurement at 10 Hz.
 *
 * @return LPS22HH_OK if initialization was successful.
 */
LPS22HH_Status_t LPS22HH_Init(void);

/**
 * @brief Checks whether the LPS22HH is present and
 *        verifies the WHO_AM_I register.
 *
 * @return LPS22HH_OK if the expected device is detected.
 */
LPS22HH_Status_t LPS22HH_CheckDevice(void);

/**
 * @brief Reads the current pressure measurement.
 *
 * @param pressure_hpa Pointer receiving pressure in hPa.
 *
 * @return LPS22HH_OK if the measurement was successfully read.
 */
LPS22HH_Status_t LPS22HH_ReadPressure(float *pressure_hpa);

#ifdef __cplusplus
}
#endif

#endif /* LPS22HH_H */