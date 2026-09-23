#include "vin_monitor.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

#define VIN_MONITOR_ADC_MAX_VALUE       4095.0f
#define VIN_MONITOR_ADC_REFERENCE_V     3.3f

#define VIN_MONITOR_R_TOP_OHM           110000.0f
#define VIN_MONITOR_R_BOTTOM_OHM        10000.0f

#define VIN_MONITOR_DIVIDER_RATIO       \
    ((VIN_MONITOR_R_TOP_OHM + VIN_MONITOR_R_BOTTOM_OHM) / \
     VIN_MONITOR_R_BOTTOM_OHM)

#define VIN_MONITOR_ADC_TIMEOUT_MS      100U


VIN_MONITOR_Status_t VIN_MONITOR_Init(void)
{
    /*
     * ADC1 is initialized by CubeMX before this driver is used.
     * No additional peripheral configuration is required here.
     */
    return VIN_MONITOR_OK;
}


VIN_MONITOR_Status_t VIN_MONITOR_ReadRaw(unsigned int *raw_adc)
{
    HAL_StatusTypeDef hal_status;

    if (raw_adc == NULL)
    {
        return VIN_MONITOR_INVALID_ARG;
    }

    hal_status = HAL_ADC_Start(&hadc1);

    if (hal_status != HAL_OK)
    {
        return VIN_MONITOR_ERROR;
    }

    hal_status = HAL_ADC_PollForConversion(
        &hadc1,
        VIN_MONITOR_ADC_TIMEOUT_MS);

    if (hal_status == HAL_TIMEOUT)
    {
        HAL_ADC_Stop(&hadc1);
        return VIN_MONITOR_TIMEOUT;
    }

    if (hal_status != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        return VIN_MONITOR_ERROR;
    }

    *raw_adc = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    return VIN_MONITOR_OK;
}


VIN_MONITOR_Status_t VIN_MONITOR_ReadVoltage(float *voltage_v)
{
    unsigned int raw_adc;
    VIN_MONITOR_Status_t status;

    if (voltage_v == NULL)
    {
        return VIN_MONITOR_INVALID_ARG;
    }

    status = VIN_MONITOR_ReadRaw(&raw_adc);

    if (status != VIN_MONITOR_OK)
    {
        return status;
    }

    *voltage_v =
        ((float)raw_adc / VIN_MONITOR_ADC_MAX_VALUE) *
        VIN_MONITOR_ADC_REFERENCE_V *
        VIN_MONITOR_DIVIDER_RATIO;

    return VIN_MONITOR_OK;
}