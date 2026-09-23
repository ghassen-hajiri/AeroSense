#include "stts22h.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c2;

#define STTS22H_I2C_ADDRESS        (0x3FU << 1)

#define STTS22H_REG_WHO_AM_I       0x01U
#define STTS22H_WHO_AM_I_VALUE     0xA0U

#define STTS22H_REG_TEMP_L         0x06U

#define STTS22H_I2C_TIMEOUT_MS     100U

STTS22H_Status_t STTS22H_CheckDevice(void)
{
    uint8_t who_am_i = 0U;

    if (HAL_I2C_IsDeviceReady(
            &hi2c2,
            STTS22H_I2C_ADDRESS,
            3U,
            STTS22H_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return STTS22H_DEVICE_NOT_FOUND;
    }

    if (HAL_I2C_Mem_Read(
            &hi2c2,
            STTS22H_I2C_ADDRESS,
            STTS22H_REG_WHO_AM_I,
            I2C_MEMADD_SIZE_8BIT,
            &who_am_i,
            1U,
            STTS22H_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return STTS22H_ERROR;
    }

    if (who_am_i != STTS22H_WHO_AM_I_VALUE)
    {
        return STTS22H_DEVICE_NOT_FOUND;
    }

    return STTS22H_OK;
}


STTS22H_Status_t STTS22H_Init(void)
{
    return STTS22H_CheckDevice();
}


STTS22H_Status_t STTS22H_ReadTemperature(float *temperature_c)
{
    uint8_t raw_data[2];
    int16_t raw_temperature;

    if (temperature_c == NULL)
    {
        return STTS22H_ERROR;
    }

    if (HAL_I2C_Mem_Read(
            &hi2c2,
            STTS22H_I2C_ADDRESS,
            STTS22H_REG_TEMP_L,
            I2C_MEMADD_SIZE_8BIT,
            raw_data,
            2U,
            STTS22H_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return STTS22H_ERROR;
    }

    raw_temperature =
        (int16_t)(((uint16_t)raw_data[1] << 8) |
                   (uint16_t)raw_data[0]);

    *temperature_c = (float)raw_temperature / 100.0f;

    return STTS22H_OK;
}