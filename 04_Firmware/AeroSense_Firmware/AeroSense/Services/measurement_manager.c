#include "measurement_manager.h"

#include "stts22h.h"
#include "lps22hh.h"
#include "lis2dw12.h"
#include "vin_monitor.h"
#include "diagnostic_manager.h"
#include "configuration_manager.h"


#define MEASUREMENT_FAILURE_THRESHOLD      3U
#define MEASUREMENT_SUCCESS_THRESHOLD      3U

#define VIN_UNDERVOLTAGE_HYSTERESIS_V      0.5f

typedef struct
{
    uint8_t failure_count;
    uint8_t success_count;

} MEASUREMENT_MANAGER_DiagnosticCounter_t;


static MEASUREMENT_MANAGER_Data_t measurement_data;

static MEASUREMENT_MANAGER_DiagnosticCounter_t temperature_counter;
static MEASUREMENT_MANAGER_DiagnosticCounter_t pressure_counter;
static MEASUREMENT_MANAGER_DiagnosticCounter_t acceleration_counter;
static MEASUREMENT_MANAGER_DiagnosticCounter_t vin_counter;
static MEASUREMENT_MANAGER_DiagnosticCounter_t vin_undervoltage_counter;


/*
 * Processes a successful cyclic measurement.
 *
 * A confirmed diagnostic fault is cleared only after
 * three consecutive successful acquisitions.
 */
static void MEASUREMENT_MANAGER_ProcessSuccess(
    MEASUREMENT_MANAGER_DiagnosticCounter_t *counter,
    DIAGNOSTIC_FaultId_t fault_id)
{
    counter->failure_count = 0U;

    if (counter->success_count < MEASUREMENT_SUCCESS_THRESHOLD)
    {
        counter->success_count++;
    }

    if (counter->success_count >= MEASUREMENT_SUCCESS_THRESHOLD)
    {
        DIAGNOSTIC_ClearFault(fault_id);
    }
}


/*
 * Processes a failed cyclic measurement.
 *
 * A diagnostic fault is confirmed only after
 * three consecutive failed acquisitions.
 */
static void MEASUREMENT_MANAGER_ProcessFailure(
    MEASUREMENT_MANAGER_DiagnosticCounter_t *counter,
    DIAGNOSTIC_FaultId_t fault_id)
{
    counter->success_count = 0U;

    if (counter->failure_count < MEASUREMENT_FAILURE_THRESHOLD)
    {
        counter->failure_count++;
    }

    if (counter->failure_count >= MEASUREMENT_FAILURE_THRESHOLD)
    {
        DIAGNOSTIC_SetFault(
            fault_id,
            DIAGNOSTIC_SEVERITY_DEGRADED);
    }
}


MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_Init(void)
{
    MEASUREMENT_MANAGER_Status_t overall_status =
        MEASUREMENT_MANAGER_OK;


    /*
     * Initialize measurement data.
     */
    measurement_data.temperature_c = 0.0f;
    measurement_data.pressure_hpa = 0.0f;

    measurement_data.acceleration_x_g = 0.0f;
    measurement_data.acceleration_y_g = 0.0f;
    measurement_data.acceleration_z_g = 0.0f;

    measurement_data.vin_voltage = 0.0f;

    measurement_data.temperature_valid = 0U;
    measurement_data.pressure_valid = 0U;
    measurement_data.acceleration_valid = 0U;
    measurement_data.vin_valid = 0U;

    


    /*
     * Initialize diagnostic confirmation counters.
     */
    temperature_counter.failure_count = 0U;
    temperature_counter.success_count = 0U;

    pressure_counter.failure_count = 0U;
    pressure_counter.success_count = 0U;

    acceleration_counter.failure_count = 0U;
    acceleration_counter.success_count = 0U;

    vin_counter.failure_count = 0U;
    vin_counter.success_count = 0U;

    vin_undervoltage_counter.failure_count = 0U;
    vin_undervoltage_counter.success_count = 0U;


    /*
     * Initialize temperature sensor.
     */
    if (STTS22H_Init() != STTS22H_OK)
    {
        DIAGNOSTIC_SetFault(
            DIAGNOSTIC_FAULT_TEMPERATURE,
            DIAGNOSTIC_SEVERITY_DEGRADED);

        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    /*
     * Initialize pressure sensor.
     */
    if (LPS22HH_Init() != LPS22HH_OK)
    {
        DIAGNOSTIC_SetFault(
            DIAGNOSTIC_FAULT_PRESSURE,
            DIAGNOSTIC_SEVERITY_DEGRADED);

        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    /*
     * Initialize acceleration sensor.
     */
    if (LIS2DW12_Init() != LIS2DW12_OK)
    {
        DIAGNOSTIC_SetFault(
            DIAGNOSTIC_FAULT_ACCELERATION,
            DIAGNOSTIC_SEVERITY_DEGRADED);

        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    /*
     * Initialize VIN monitoring.
     */
    if (VIN_MONITOR_Init() != VIN_MONITOR_OK)
    {
        DIAGNOSTIC_SetFault(
            DIAGNOSTIC_FAULT_VIN_ADC,
            DIAGNOSTIC_SEVERITY_DEGRADED);

        overall_status = MEASUREMENT_MANAGER_ERROR;
    }


    return overall_status;
}


MEASUREMENT_MANAGER_Status_t
MEASUREMENT_MANAGER_UpdateTemperature(void)
{
    if (STTS22H_ReadTemperature(
            &measurement_data.temperature_c) == STTS22H_OK)
    {
        measurement_data.temperature_valid = 1U;

        MEASUREMENT_MANAGER_ProcessSuccess(
            &temperature_counter,
            DIAGNOSTIC_FAULT_TEMPERATURE);

        return MEASUREMENT_MANAGER_OK;
    }


    measurement_data.temperature_valid = 0U;

    MEASUREMENT_MANAGER_ProcessFailure(
        &temperature_counter,
        DIAGNOSTIC_FAULT_TEMPERATURE);

    return MEASUREMENT_MANAGER_ERROR;
}


MEASUREMENT_MANAGER_Status_t
MEASUREMENT_MANAGER_UpdatePressure(void)
{
    if (LPS22HH_ReadPressure(
            &measurement_data.pressure_hpa) == LPS22HH_OK)
    {
        measurement_data.pressure_valid = 1U;

        MEASUREMENT_MANAGER_ProcessSuccess(
            &pressure_counter,
            DIAGNOSTIC_FAULT_PRESSURE);

        return MEASUREMENT_MANAGER_OK;
    }


    measurement_data.pressure_valid = 0U;

    MEASUREMENT_MANAGER_ProcessFailure(
        &pressure_counter,
        DIAGNOSTIC_FAULT_PRESSURE);

    return MEASUREMENT_MANAGER_ERROR;
}


MEASUREMENT_MANAGER_Status_t
MEASUREMENT_MANAGER_UpdateAcceleration(void)
{
    LIS2DW12_Acceleration_t acceleration;


    if (LIS2DW12_ReadAcceleration(
            &acceleration) == LIS2DW12_OK)
    {
        measurement_data.acceleration_x_g =
            acceleration.x_g;

        measurement_data.acceleration_y_g =
            acceleration.y_g;

        measurement_data.acceleration_z_g =
            acceleration.z_g;

        measurement_data.acceleration_valid = 1U;

        MEASUREMENT_MANAGER_ProcessSuccess(
            &acceleration_counter,
            DIAGNOSTIC_FAULT_ACCELERATION);

        return MEASUREMENT_MANAGER_OK;
    }


    measurement_data.acceleration_valid = 0U;

    MEASUREMENT_MANAGER_ProcessFailure(
        &acceleration_counter,
        DIAGNOSTIC_FAULT_ACCELERATION);

    return MEASUREMENT_MANAGER_ERROR;
}


MEASUREMENT_MANAGER_Status_t MEASUREMENT_MANAGER_UpdateVIN(void)
{
    float voltage_v;
    float undervoltage_threshold_v;
    float recovery_threshold_v;

    undervoltage_threshold_v =
        CONFIGURATION_MANAGER_GetVinUndervoltageThreshold();

    recovery_threshold_v =
        undervoltage_threshold_v +
        VIN_UNDERVOLTAGE_HYSTERESIS_V;

    if (VIN_MONITOR_ReadVoltage(&voltage_v) == VIN_MONITOR_OK)
    {
        measurement_data.vin_voltage = voltage_v;
        measurement_data.vin_valid = 1U;

        MEASUREMENT_MANAGER_ProcessSuccess(
            &vin_counter,
            DIAGNOSTIC_FAULT_VIN_ADC);

        if (voltage_v < undervoltage_threshold_v)
        {
            MEASUREMENT_MANAGER_ProcessFailure(
                &vin_undervoltage_counter,
                DIAGNOSTIC_FAULT_VIN_UNDERVOLTAGE);
        }
        else if (voltage_v >= recovery_threshold_v)
        {
            MEASUREMENT_MANAGER_ProcessSuccess(
                &vin_undervoltage_counter,
                DIAGNOSTIC_FAULT_VIN_UNDERVOLTAGE);
        }
        else
        {
            /*
             * Hysteresis region:
             * keep the current diagnostic state.
             */
        }

        return MEASUREMENT_MANAGER_OK;
    }

    measurement_data.vin_valid = 0U;

    MEASUREMENT_MANAGER_ProcessFailure(
        &vin_counter,
        DIAGNOSTIC_FAULT_VIN_ADC);

    return MEASUREMENT_MANAGER_ERROR;
}

MEASUREMENT_MANAGER_Status_t
MEASUREMENT_MANAGER_ProcessRecovery(void)
{
    MEASUREMENT_MANAGER_Status_t overall_status =
        MEASUREMENT_MANAGER_OK;


    /*
     * Temperature sensor recovery.
     */
    if (DIAGNOSTIC_IsFaultActive(
            DIAGNOSTIC_FAULT_TEMPERATURE) != 0U)
    {
        if (STTS22H_Init() != STTS22H_OK)
        {
            overall_status = MEASUREMENT_MANAGER_ERROR;
        }
    }


    /*
     * Pressure sensor recovery.
     */
    if (DIAGNOSTIC_IsFaultActive(
            DIAGNOSTIC_FAULT_PRESSURE) != 0U)
    {
        if (LPS22HH_Init() != LPS22HH_OK)
        {
            overall_status = MEASUREMENT_MANAGER_ERROR;
        }
    }


    /*
     * Acceleration sensor recovery.
     */
    if (DIAGNOSTIC_IsFaultActive(
            DIAGNOSTIC_FAULT_ACCELERATION) != 0U)
    {
        if (LIS2DW12_Init() != LIS2DW12_OK)
        {
            overall_status = MEASUREMENT_MANAGER_ERROR;
        }
    }


    /*
     * VIN monitoring recovery.
     */
    if (DIAGNOSTIC_IsFaultActive(
            DIAGNOSTIC_FAULT_VIN_ADC) != 0U)
    {
        if (VIN_MONITOR_Init() != VIN_MONITOR_OK)
        {
            overall_status = MEASUREMENT_MANAGER_ERROR;
        }
    }


    return overall_status;
}

const MEASUREMENT_MANAGER_Data_t *
MEASUREMENT_MANAGER_GetData(void)
{
    return &measurement_data;
}