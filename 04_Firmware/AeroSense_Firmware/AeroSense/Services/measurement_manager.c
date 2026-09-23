#include "measurement_manager.h"

#include "stts22h.h"
#include "lps22hh.h"
#include "lis2dw12.h"
#include "vin_monitor.h"


static MEASUREMENT_MANAGER_Data_t measurement_data;


MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_Init(void)
{
    MEASUREMENT_MANAGER_Status_t overall_status =
        MEASUREMENT_MANAGER_OK;

    measurement_data.temperature_valid = 0U;
    measurement_data.pressure_valid = 0U;
    measurement_data.acceleration_valid = 0U;
    measurement_data.vin_valid = 0U;


    if (STTS22H_Init() != STTS22H_OK)
    {
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }

    if (LPS22HH_Init() != LPS22HH_OK)
    {
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }

    if (LIS2DW12_Init() != LIS2DW12_OK)
    {
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }

    if (VIN_MONITOR_Init() != VIN_MONITOR_OK)
    {
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }

    return overall_status;
}


MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_Update(void)
{
    MEASUREMENT_MANAGER_Status_t overall_status =
        MEASUREMENT_MANAGER_OK;

    LIS2DW12_Acceleration_t acceleration;


    if (STTS22H_ReadTemperature(
            &measurement_data.temperature_c) == STTS22H_OK)
    {
        measurement_data.temperature_valid = 1U;
    }
    else
    {
        measurement_data.temperature_valid = 0U;
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    if (LPS22HH_ReadPressure(
            &measurement_data.pressure_hpa) == LPS22HH_OK)
    {
        measurement_data.pressure_valid = 1U;
    }
    else
    {
        measurement_data.pressure_valid = 0U;
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    if (LIS2DW12_ReadAcceleration(
            &acceleration) == LIS2DW12_OK)
    {
        measurement_data.acceleration_x_g = acceleration.x_g;
        measurement_data.acceleration_y_g = acceleration.y_g;
        measurement_data.acceleration_z_g = acceleration.z_g;

        measurement_data.acceleration_valid = 1U;
    }
    else
    {
        measurement_data.acceleration_valid = 0U;
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    if (VIN_MONITOR_ReadVoltage(
            &measurement_data.vin_voltage) == VIN_MONITOR_OK)
    {
        measurement_data.vin_valid = 1U;
    }
    else
    {
        measurement_data.vin_valid = 0U;
        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    return overall_status;
}


const MEASUREMENT_MANAGER_Data_t *MEASUREMENT_MANAGER_GetData(void)
{
    return &measurement_data;
}