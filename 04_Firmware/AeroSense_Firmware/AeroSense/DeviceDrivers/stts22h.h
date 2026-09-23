#ifndef STTS22H_H
#define STTS22H_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    STTS22H_OK = 0,
    STTS22H_ERROR,
    STTS22H_TIMEOUT,
    STTS22H_DEVICE_NOT_FOUND
} STTS22H_Status_t;

/**
 * @brief Initializes the STTS22H temperature sensor.
 *
 * @return STTS22H_OK if initialization was successful.
 */
STTS22H_Status_t STTS22H_Init(void);

/**
 * @brief Checks whether the STTS22H responds on the I2C bus.
 *
 * @return STTS22H_OK if the device is reachable.
 */
STTS22H_Status_t STTS22H_CheckDevice(void);

/**
 * @brief Reads the current temperature.
 *
 * @param temperature_c Pointer receiving temperature in degrees Celsius.
 *
 * @return STTS22H_OK if the measurement was successfully read.
 */
STTS22H_Status_t STTS22H_ReadTemperature(float *temperature_c);

#ifdef __cplusplus
}
#endif

#endif /* STTS22H_H */