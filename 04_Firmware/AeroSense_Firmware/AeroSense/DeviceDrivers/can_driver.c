#include "can_driver.h"
#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;


CAN_DRIVER_Status_t CAN_DRIVER_Init(void)
{
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        return CAN_DRIVER_ERROR;
    }

    return CAN_DRIVER_OK;
}


CAN_DRIVER_Status_t CAN_DRIVER_Transmit(
    const CAN_DRIVER_Message_t *message)
{
    FDCAN_TxHeaderTypeDef tx_header;

    if (message == NULL)
    {
        return CAN_DRIVER_INVALID_ARG;
    }

    if (message->length > CAN_DRIVER_MAX_DATA_LENGTH)
    {
        return CAN_DRIVER_INVALID_ARG;
    }

    if (message->id > 0x7FFU)
    {
        return CAN_DRIVER_INVALID_ARG;
    }

tx_header.Identifier = message->id;
tx_header.IdType = FDCAN_STANDARD_ID;
tx_header.TxFrameType = FDCAN_DATA_FRAME;
tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
tx_header.BitRateSwitch = FDCAN_BRS_OFF;
tx_header.FDFormat = FDCAN_CLASSIC_CAN;
tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
tx_header.MessageMarker = 0U;

    switch (message->length)
    {
        case 0U:
            tx_header.DataLength = FDCAN_DLC_BYTES_0;
            break;

        case 1U:
            tx_header.DataLength = FDCAN_DLC_BYTES_1;
            break;

        case 2U:
            tx_header.DataLength = FDCAN_DLC_BYTES_2;
            break;

        case 3U:
            tx_header.DataLength = FDCAN_DLC_BYTES_3;
            break;

        case 4U:
            tx_header.DataLength = FDCAN_DLC_BYTES_4;
            break;

        case 5U:
            tx_header.DataLength = FDCAN_DLC_BYTES_5;
            break;

        case 6U:
            tx_header.DataLength = FDCAN_DLC_BYTES_6;
            break;

        case 7U:
            tx_header.DataLength = FDCAN_DLC_BYTES_7;
            break;

        case 8U:
            tx_header.DataLength = FDCAN_DLC_BYTES_8;
            break;

        default:
            return CAN_DRIVER_INVALID_ARG;
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(
            &hfdcan1,
            &tx_header,
            (uint8_t *)message->data) != HAL_OK)
    {
        return CAN_DRIVER_ERROR;
    }

    return CAN_DRIVER_OK;
}