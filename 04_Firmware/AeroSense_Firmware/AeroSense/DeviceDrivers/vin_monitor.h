#ifndef VIN_MONITOR_H
#define VIN_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    VIN_MONITOR_OK = 0,
    VIN_MONITOR_ERROR,
    VIN_MONITOR_TIMEOUT,
    VIN_MONITOR_INVALID_ARG
} VIN_MONITOR_Status_t;

/**
 * @brief Initializes the VIN monitoring driver.
 *
 * @return VIN_MONITOR_OK if initialization was successful.
 */
VIN_MONITOR_Status_t VIN_MONITOR_Init(void);

/**
 * @brief Reads the raw ADC value.
 *
 * @param raw_adc Pointer receiving the raw 12-bit ADC value.
 *
 * @return VIN_MONITOR_OK if the ADC conversion was successful.
 */
VIN_MONITOR_Status_t VIN_MONITOR_ReadRaw(unsigned int *raw_adc);

/**
 * @brief Reads and converts the input supply voltage.
 *
 * @param voltage_v Pointer receiving VIN in volts.
 *
 * @return VIN_MONITOR_OK if the measurement was successful.
 */
VIN_MONITOR_Status_t VIN_MONITOR_ReadVoltage(float *voltage_v);

#ifdef __cplusplus
}
#endif

#endif /* VIN_MONITOR_H */