#include "lps22hh.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c2;

/*
 * LPS22HH I2C address.
 *
 * SA0 = GND -> 7-bit address 0x5C
 * STM32 HAL expects the address shifted left by one bit.
 */
#define LPS22HH_I2C_ADDRESS          (0x5CU << 1)

#define LPS22HH_REG_WHO_AM_I         0x0FU
#define LPS22HH_WHO_AM_I_VALUE       0xB3U

#define LPS22HH_REG_CTRL_REG1        0x10U

#define LPS22HH_REG_PRESS_OUT_XL     0x28U

#define LPS22HH_CTRL_REG1_10HZ_BDU   0x22U

#define LPS22HH_I2C_TIMEOUT_MS       100U


LPS22HH_Status_t LPS22HH_CheckDevice(void)
{
    uint8_t who_am_i = 0U;

    if (HAL_I2C_IsDeviceReady(
            &hi2c2,
            LPS22HH_I2C_ADDRESS,
            3U,
            LPS22HH_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return LPS22HH_DEVICE_NOT_FOUND;
    }

    if (HAL_I2C_Mem_Read(
            &hi2c2,
            LPS22HH_I2C_ADDRESS,
            LPS22HH_REG_WHO_AM_I,
            I2C_MEMADD_SIZE_8BIT,
            &who_am_i,
            1U,
            LPS22HH_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return LPS22HH_ERROR;
    }

    if (who_am_i != LPS22HH_WHO_AM_I_VALUE)
    {
        return LPS22HH_DEVICE_NOT_FOUND;
    }

    return LPS22HH_OK;
}


LPS22HH_Status_t LPS22HH_Init(void)
{
    uint8_t ctrl_reg1 = LPS22HH_CTRL_REG1_10HZ_BDU;

    if (LPS22HH_CheckDevice() != LPS22HH_OK)
    {
        return LPS22HH_DEVICE_NOT_FOUND;
    }

    /*
     * CTRL_REG1:
     *
     * ODR[2:0] = 010 -> 10 Hz continuous mode
     * BDU      = 1   -> Block Data Update enabled
     */
    if (HAL_I2C_Mem_Write(
            &hi2c2,
            LPS22HH_I2C_ADDRESS,
            LPS22HH_REG_CTRL_REG1,
            I2C_MEMADD_SIZE_8BIT,
            &ctrl_reg1,
            1U,
            LPS22HH_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return LPS22HH_ERROR;
    }

    return LPS22HH_OK;
}


LPS22HH_Status_t LPS22HH_ReadPressure(float *pressure_hpa)
{
    uint8_t raw_data[3];
    int32_t raw_pressure;

    if (pressure_hpa == NULL)
    {
        return LPS22HH_INVALID_ARG;
    }

    if (HAL_I2C_Mem_Read(
            &hi2c2,
            LPS22HH_I2C_ADDRESS,
            LPS22HH_REG_PRESS_OUT_XL,
            I2C_MEMADD_SIZE_8BIT,
            raw_data,
            3U,
            LPS22HH_I2C_TIMEOUT_MS) != HAL_OK)
    {
        return LPS22HH_ERROR;
    }

    raw_pressure =
        ((int32_t)raw_data[2] << 16) |
        ((int32_t)raw_data[1] << 8)  |
        ((int32_t)raw_data[0]);

    /*
     * Sign-extend the 24-bit two's-complement value.
     */
    if ((raw_pressure & 0x00800000L) != 0)
    {
        raw_pressure |= (int32_t)0xFF000000L;
    }

    /*
     * LPS22HH pressure sensitivity:
     * 4096 LSB/hPa
     */
    *pressure_hpa = (float)raw_pressure / 4096.0f;

    return LPS22HH_OK;
}