#include "usb_driver.h"

#include "usbd_cdc_if.h"
#include <string.h>

static uint8_t usb_rx_buffer[USB_DRIVER_RX_BUFFER_SIZE];
static volatile uint16_t usb_rx_length = 0U;
static volatile uint8_t usb_data_available = 0U;

USB_DRIVER_Status_t USB_DRIVER_Init(void)
{
    usb_rx_length = 0U;
    usb_data_available = 0U;

    return USB_DRIVER_OK;
}

USB_DRIVER_Status_t USB_DRIVER_Transmit(
    const uint8_t *data,
    uint16_t length)
{
    uint8_t usb_status;

    if ((data == NULL) || (length == 0U))
    {
        return USB_DRIVER_INVALID_ARG;
    }

    usb_status = CDC_Transmit_FS((uint8_t *)data, length);

    if (usb_status == USBD_OK)
    {
        return USB_DRIVER_OK;
    }

    if (usb_status == USBD_BUSY)
    {
        return USB_DRIVER_BUSY;
    }

    return USB_DRIVER_ERROR;
}

void USB_DRIVER_ReceiveCallback(
    const uint8_t *data,
    uint32_t length)
{
    uint32_t copy_length;

    if ((data == NULL) || (length == 0U))
    {
        return;
    }

    copy_length = length;

    if (copy_length > USB_DRIVER_RX_BUFFER_SIZE)
    {
        copy_length = USB_DRIVER_RX_BUFFER_SIZE;
    }

    memcpy(usb_rx_buffer, data, copy_length);

    usb_rx_length = (uint16_t)copy_length;
    usb_data_available = 1U;
}

uint8_t USB_DRIVER_IsDataAvailable(void)
{
    return usb_data_available;
}

uint16_t USB_DRIVER_Read(
    uint8_t *data,
    uint16_t max_length)
{
    uint16_t copy_length;

    if ((data == NULL) || (max_length == 0U))
    {
        return 0U;
    }

    if (usb_data_available == 0U)
    {
        return 0U;
    }

    copy_length = usb_rx_length;

    if (copy_length > max_length)
    {
        copy_length = max_length;
    }

    memcpy(data, usb_rx_buffer, copy_length);

    usb_rx_length = 0U;
    usb_data_available = 0U;

    return copy_length;
}