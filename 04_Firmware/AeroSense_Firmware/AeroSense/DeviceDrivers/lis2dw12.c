#include "lis2dw12.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define LIS2DW12_REG_WHO_AM_I       0x0FU
#define LIS2DW12_WHO_AM_I_VALUE     0x44U

#define LIS2DW12_REG_CTRL1          0x20U
#define LIS2DW12_REG_CTRL2          0x21U
#define LIS2DW12_REG_CTRL6          0x25U

#define LIS2DW12_REG_OUT_X_L        0x28U

#define LIS2DW12_SPI_READ           0x80U

#define LIS2DW12_SPI_TIMEOUT_MS     100U

/*
 * CTRL1:
 * ODR  = 0100 -> 50 Hz
 * MODE = 01   -> High-performance mode
 * LP_MODE = 00
 */
#define LIS2DW12_CTRL1_CONFIG       0x44U

/*
 * CTRL2:
 * BDU        = 1
 * IF_ADD_INC = 1
 */
#define LIS2DW12_CTRL2_CONFIG       0x0CU

/*
 * CTRL6:
 * FS = 00 -> +/-2 g
 */
#define LIS2DW12_CTRL6_CONFIG       0x00U

#define LIS2DW12_SENSITIVITY_G      0.000244f


static void LIS2DW12_Select(void)
{
    HAL_GPIO_WritePin(
        SPI_CS_GPIO_Port,
        SPI_CS_Pin,
        GPIO_PIN_RESET);
}


static void LIS2DW12_Deselect(void)
{
    HAL_GPIO_WritePin(
        SPI_CS_GPIO_Port,
        SPI_CS_Pin,
        GPIO_PIN_SET);
}


static HAL_StatusTypeDef LIS2DW12_ReadRegisters(
    uint8_t reg,
    uint8_t *data,
    uint16_t length)
{
    uint8_t command = reg | LIS2DW12_SPI_READ;
    HAL_StatusTypeDef status;

    LIS2DW12_Select();

    status = HAL_SPI_Transmit(
        &hspi1,
        &command,
        1U,
        LIS2DW12_SPI_TIMEOUT_MS);

    if (status == HAL_OK)
    {
        status = HAL_SPI_Receive(
            &hspi1,
            data,
            length,
            LIS2DW12_SPI_TIMEOUT_MS);
    }

    LIS2DW12_Deselect();

    return status;
}


static HAL_StatusTypeDef LIS2DW12_WriteRegister(
    uint8_t reg,
    uint8_t value)
{
    uint8_t data[2];

    data[0] = reg & 0x7FU;
    data[1] = value;

    LIS2DW12_Select();

    HAL_StatusTypeDef status = HAL_SPI_Transmit(
        &hspi1,
        data,
        2U,
        LIS2DW12_SPI_TIMEOUT_MS);

    LIS2DW12_Deselect();

    return status;
}


LIS2DW12_Status_t LIS2DW12_CheckDevice(void)
{
    uint8_t who_am_i = 0U;

    if (LIS2DW12_ReadRegisters(
            LIS2DW12_REG_WHO_AM_I,
            &who_am_i,
            1U) != HAL_OK)
    {
        return LIS2DW12_ERROR;
    }

    if (who_am_i != LIS2DW12_WHO_AM_I_VALUE)
    {
        return LIS2DW12_DEVICE_NOT_FOUND;
    }

    return LIS2DW12_OK;
}


LIS2DW12_Status_t LIS2DW12_Init(void)
{
    if (LIS2DW12_CheckDevice() != LIS2DW12_OK)
    {
        return LIS2DW12_DEVICE_NOT_FOUND;
    }

    if (LIS2DW12_WriteRegister(
            LIS2DW12_REG_CTRL2,
            LIS2DW12_CTRL2_CONFIG) != HAL_OK)
    {
        return LIS2DW12_ERROR;
    }

    if (LIS2DW12_WriteRegister(
            LIS2DW12_REG_CTRL6,
            LIS2DW12_CTRL6_CONFIG) != HAL_OK)
    {
        return LIS2DW12_ERROR;
    }

    if (LIS2DW12_WriteRegister(
            LIS2DW12_REG_CTRL1,
            LIS2DW12_CTRL1_CONFIG) != HAL_OK)
    {
        return LIS2DW12_ERROR;
    }

    return LIS2DW12_OK;
}


LIS2DW12_Status_t LIS2DW12_ReadAcceleration(
    LIS2DW12_Acceleration_t *acceleration)
{
    uint8_t raw_data[6];

    int16_t raw_x;
    int16_t raw_y;
    int16_t raw_z;

    if (acceleration == NULL)
    {
        return LIS2DW12_INVALID_ARG;
    }

    if (LIS2DW12_ReadRegisters(
            LIS2DW12_REG_OUT_X_L,
            raw_data,
            6U) != HAL_OK)
    {
        return LIS2DW12_ERROR;
    }

    raw_x = (int16_t)(
        ((uint16_t)raw_data[1] << 8) |
        raw_data[0]);

    raw_y = (int16_t)(
        ((uint16_t)raw_data[3] << 8) |
        raw_data[2]);

    raw_z = (int16_t)(
        ((uint16_t)raw_data[5] << 8) |
        raw_data[4]);

    acceleration->x_g =
        (float)raw_x * LIS2DW12_SENSITIVITY_G;

    acceleration->y_g =
        (float)raw_y * LIS2DW12_SENSITIVITY_G;

    acceleration->z_g =
        (float)raw_z * LIS2DW12_SENSITIVITY_G;

    return LIS2DW12_OK;
}