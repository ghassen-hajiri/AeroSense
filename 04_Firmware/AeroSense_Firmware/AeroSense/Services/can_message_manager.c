#include "can_message_manager.h"

#include "can_driver.h"
#include "measurement_manager.h"
#include "diagnostic_manager.h"
#include "system_manager.h"

#include <stddef.h>
#include <stdint.h>


/* CAN message identifiers */
#define CANMSG_ID_ENVIRONMENT        0x100U
#define CANMSG_ID_ACCELERATION       0x101U
#define CANMSG_ID_POWER              0x102U


/* CAN payload lengths */
#define CANMSG_ENV_LENGTH            8U
#define CANMSG_ACCEL_LENGTH          8U
#define CANMSG_POWER_LENGTH          8U


/* Physical value scaling */
#define CANMSG_TEMP_SCALE            100.0f
#define CANMSG_PRESSURE_SCALE        100.0f
#define CANMSG_ACCEL_SCALE           1000.0f
#define CANMSG_VIN_SCALE             1000.0f


/* Environment validity flags */
#define CANMSG_VALID_TEMPERATURE     (1U << 0)
#define CANMSG_VALID_PRESSURE        (1U << 1)

/* Acceleration validity flags */
#define CANMSG_VALID_ACCELERATION    (1U << 0)

/* Power validity flags */
#define CANMSG_VALID_VIN             (1U << 0)

/* System Status*/

#define CANMSG_ID_SYSTEM_STATUS       0x103U
#define CANMSG_STATUS_LENGTH          8U


CANMSG_Status_t CANMSG_Init(void)
{
    if (CAN_DRIVER_Init() != CAN_DRIVER_OK)
    {
        return CANMSG_ERROR;
    }

    return CANMSG_OK;
}


CANMSG_Status_t CANMSG_TransmitEnvironment(void)
{
    CAN_DRIVER_Message_t message = {0};

    const MEASUREMENT_MANAGER_Data_t *data =
        MEASUREMENT_MANAGER_GetData();

    int16_t temperature_raw;
    uint32_t pressure_raw;
    uint8_t validity = 0U;

    if (data == NULL)
    {
        return CANMSG_INVALID_ARG;
    }

    /*
     * Temperature:
     * Physical unit: degC
     * CAN resolution: 0.01 degC/bit
     */
    temperature_raw =
        (int16_t)(data->temperature_c * CANMSG_TEMP_SCALE);

    /*
     * Pressure:
     * Physical unit: hPa
     * CAN resolution: 0.01 hPa/bit
     */
    pressure_raw =
        (uint32_t)(data->pressure_hpa * CANMSG_PRESSURE_SCALE);


    if (data->temperature_valid != 0U)
    {
        validity |= CANMSG_VALID_TEMPERATURE;
    }

    if (data->pressure_valid != 0U)
    {
        validity |= CANMSG_VALID_PRESSURE;
    }


    message.id = CANMSG_ID_ENVIRONMENT;
    message.length = CANMSG_ENV_LENGTH;


    /*
     * Byte 0-1:
     * Temperature
     * int16
     * Little-endian
     */
    message.data[0] =
        (uint8_t)((uint16_t)temperature_raw & 0xFFU);

    message.data[1] =
        (uint8_t)(((uint16_t)temperature_raw >> 8) & 0xFFU);


    /*
     * Byte 2-5:
     * Pressure
     * uint32
     * Little-endian
     */
    message.data[2] =
        (uint8_t)(pressure_raw & 0xFFU);

    message.data[3] =
        (uint8_t)((pressure_raw >> 8) & 0xFFU);

    message.data[4] =
        (uint8_t)((pressure_raw >> 16) & 0xFFU);

    message.data[5] =
        (uint8_t)((pressure_raw >> 24) & 0xFFU);


    /*
     * Byte 6:
     * Validity flags
     *
     * Bit 0 = Temperature valid
     * Bit 1 = Pressure valid
     */
    message.data[6] = validity;


    /*
     * Byte 7:
     * Reserved
     */
    message.data[7] = 0U;


    if (CAN_DRIVER_Transmit(&message) != CAN_DRIVER_OK)
    {
        return CANMSG_ERROR;
    }

    return CANMSG_OK;
}


CANMSG_Status_t CANMSG_TransmitAcceleration(void)
{
    CAN_DRIVER_Message_t message = {0};

    const MEASUREMENT_MANAGER_Data_t *data =
        MEASUREMENT_MANAGER_GetData();

    int16_t acceleration_x_raw;
    int16_t acceleration_y_raw;
    int16_t acceleration_z_raw;
    uint8_t validity = 0U;

    if (data == NULL)
    {
        return CANMSG_INVALID_ARG;
    }


    /*
     * Acceleration:
     * Physical unit: g
     * CAN resolution: 0.001 g/bit
     */
    acceleration_x_raw =
        (int16_t)(data->acceleration_x_g * CANMSG_ACCEL_SCALE);

    acceleration_y_raw =
        (int16_t)(data->acceleration_y_g * CANMSG_ACCEL_SCALE);

    acceleration_z_raw =
        (int16_t)(data->acceleration_z_g * CANMSG_ACCEL_SCALE);


    if (data->acceleration_valid != 0U)
    {
        validity |= CANMSG_VALID_ACCELERATION;
    }


    message.id = CANMSG_ID_ACCELERATION;
    message.length = CANMSG_ACCEL_LENGTH;


    /*
     * Byte 0-1:
     * Acceleration X
     * int16
     * Little-endian
     */
    message.data[0] =
        (uint8_t)((uint16_t)acceleration_x_raw & 0xFFU);

    message.data[1] =
        (uint8_t)(((uint16_t)acceleration_x_raw >> 8) & 0xFFU);


    /*
     * Byte 2-3:
     * Acceleration Y
     * int16
     * Little-endian
     */
    message.data[2] =
        (uint8_t)((uint16_t)acceleration_y_raw & 0xFFU);

    message.data[3] =
        (uint8_t)(((uint16_t)acceleration_y_raw >> 8) & 0xFFU);


    /*
     * Byte 4-5:
     * Acceleration Z
     * int16
     * Little-endian
     */
    message.data[4] =
        (uint8_t)((uint16_t)acceleration_z_raw & 0xFFU);

    message.data[5] =
        (uint8_t)(((uint16_t)acceleration_z_raw >> 8) & 0xFFU);


    /*
     * Byte 6:
     * Validity flags
     *
     * Bit 0 = Acceleration valid
     */
    message.data[6] = validity;


    /*
     * Byte 7:
     * Reserved
     */
    message.data[7] = 0U;


    if (CAN_DRIVER_Transmit(&message) != CAN_DRIVER_OK)
    {
        return CANMSG_ERROR;
    }

    return CANMSG_OK;
}


CANMSG_Status_t CANMSG_TransmitPower(void)
{
    CAN_DRIVER_Message_t message = {0};

    const MEASUREMENT_MANAGER_Data_t *data =
        MEASUREMENT_MANAGER_GetData();

    uint16_t vin_raw;
    uint8_t validity = 0U;

    if (data == NULL)
    {
        return CANMSG_INVALID_ARG;
    }


    /*
     * Input voltage:
     * Physical unit: V
     * CAN resolution: 0.001 V/bit
     */
    vin_raw =
        (uint16_t)(data->vin_voltage * CANMSG_VIN_SCALE);


    if (data->vin_valid != 0U)
    {
        validity |= CANMSG_VALID_VIN;
    }


    message.id = CANMSG_ID_POWER;
    message.length = CANMSG_POWER_LENGTH;


    /*
     * Byte 0-1:
     * VIN
     * uint16
     * Little-endian
     */
    message.data[0] =
        (uint8_t)(vin_raw & 0xFFU);

    message.data[1] =
        (uint8_t)((vin_raw >> 8) & 0xFFU);


    /*
     * Byte 2:
     * Validity flags
     *
     * Bit 0 = VIN valid
     */
    message.data[2] = validity;


    /*
     * Byte 3-7:
     * Reserved
     */
    message.data[3] = 0U;
    message.data[4] = 0U;
    message.data[5] = 0U;
    message.data[6] = 0U;
    message.data[7] = 0U;


    if (CAN_DRIVER_Transmit(&message) != CAN_DRIVER_OK)
    {
        return CANMSG_ERROR;
    }

    return CANMSG_OK;
}


CANMSG_Status_t CANMSG_TransmitStatus(void)
{
    CAN_DRIVER_Message_t message;

    SYSTEM_MANAGER_State_t system_state;
    DIAGNOSTIC_Severity_t highest_severity;
    DIAGNOSTIC_FaultId_t primary_fault;
    uint16_t active_fault_count;


    system_state =
        SYSTEM_MANAGER_GetState();

    highest_severity =
        DIAGNOSTIC_GetHighestSeverity();

    primary_fault =
        DIAGNOSTIC_GetPrimaryFault();

    active_fault_count =
        DIAGNOSTIC_GetActiveFaultCount();


    message.id = CANMSG_ID_SYSTEM_STATUS;
    message.length = CANMSG_STATUS_LENGTH;


    /*
     * Byte 0:
     * Current system operational state
     */
    message.data[0] = (uint8_t)system_state;


    /*
     * Byte 1:
     * Highest active diagnostic severity
     */
    message.data[1] = (uint8_t)highest_severity;


    /*
     * Bytes 2-3:
     * Primary fault ID, little-endian
     */
    message.data[2] =
        (uint8_t)((uint16_t)primary_fault & 0xFFU);

    message.data[3] =
        (uint8_t)(((uint16_t)primary_fault >> 8U) & 0xFFU);


    /*
     * Bytes 4-5:
     * Number of active faults, little-endian
     */
    message.data[4] =
        (uint8_t)(active_fault_count & 0xFFU);

    message.data[5] =
        (uint8_t)((active_fault_count >> 8U) & 0xFFU);


    /*
     * Bytes 6-7:
     * Reserved for future use
     */
    message.data[6] = 0U;
    message.data[7] = 0U;


    if (CAN_DRIVER_Transmit(&message) != CAN_DRIVER_OK)
    {
        DIAGNOSTIC_SetFault(
            DIAGNOSTIC_FAULT_CAN,
            DIAGNOSTIC_SEVERITY_DEGRADED);

        return CANMSG_ERROR;
    }


    DIAGNOSTIC_ClearFault(
        DIAGNOSTIC_FAULT_CAN);


    return CANMSG_OK;
}