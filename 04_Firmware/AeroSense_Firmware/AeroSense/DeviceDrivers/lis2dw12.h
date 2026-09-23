#ifndef LIS2DW12_H
#define LIS2DW12_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LIS2DW12_OK = 0,
    LIS2DW12_ERROR,
    LIS2DW12_DEVICE_NOT_FOUND,
    LIS2DW12_INVALID_ARG
} LIS2DW12_Status_t;

typedef struct
{
    float x_g;
    float y_g;
    float z_g;
} LIS2DW12_Acceleration_t;

/**
 * @brief Initializes the LIS2DW12 accelerometer.
 *
 * @return LIS2DW12_OK if initialization was successful.
 */
LIS2DW12_Status_t LIS2DW12_Init(void);

/**
 * @brief Checks the LIS2DW12 WHO_AM_I register.
 *
 * @return LIS2DW12_OK if the expected device is detected.
 */
LIS2DW12_Status_t LIS2DW12_CheckDevice(void);

/**
 * @brief Reads acceleration on all three axes.
 *
 * @param acceleration Pointer receiving acceleration in g.
 *
 * @return LIS2DW12_OK if the measurement was successfully read.
 */
LIS2DW12_Status_t LIS2DW12_ReadAcceleration(
    LIS2DW12_Acceleration_t *acceleration);

#ifdef __cplusplus
}
#endif

#endif /* LIS2DW12_H */