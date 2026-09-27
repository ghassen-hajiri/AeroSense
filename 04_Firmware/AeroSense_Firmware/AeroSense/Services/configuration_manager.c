#include "configuration_manager.h"

#include "usb_driver.h"
#include "system_manager.h"
#include "diagnostic_manager.h"

#include <string.h>
#include <stdio.h>

#define CONFIGURATION_MANAGER_RX_SIZE 128U
#define CONFIGURATION_MANAGER_TX_SIZE 128U

#define AEROSENSE_DEVICE_ID          "AeroSense"
#define AEROSENSE_FW_VERSION         "0.1.0"
#define AEROSENSE_HW_REVISION        "RevA"

#define VIN_UV_THRESHOLD_DEFAULT_V   9.0f
#define VIN_UV_THRESHOLD_MIN_V       6.0f
#define VIN_UV_THRESHOLD_MAX_V       15.0f

static uint8_t rx_buffer[CONFIGURATION_MANAGER_RX_SIZE];
static char tx_buffer[CONFIGURATION_MANAGER_TX_SIZE];

static float vin_uv_threshold_v = VIN_UV_THRESHOLD_DEFAULT_V;

static void CONFIGURATION_MANAGER_ProcessCommand(
    uint8_t *data,
    uint16_t length);

static void CONFIGURATION_MANAGER_TransmitString(
    const char *response);

CONFIGURATION_MANAGER_Status_t CONFIGURATION_MANAGER_Init(void)
{
    memset(rx_buffer, 0, sizeof(rx_buffer));
    memset(tx_buffer, 0, sizeof(tx_buffer));

    vin_uv_threshold_v = VIN_UV_THRESHOLD_DEFAULT_V;

    return CONFIGURATION_MANAGER_OK;
}

void CONFIGURATION_MANAGER_Update(void)
{
    uint16_t length;

    if (USB_DRIVER_IsDataAvailable() == 0U)
    {
        return;
    }

    memset(rx_buffer, 0, sizeof(rx_buffer));

    length = USB_DRIVER_Read(
        rx_buffer,
        CONFIGURATION_MANAGER_RX_SIZE - 1U);

    if (length == 0U)
    {
        return;
    }

    rx_buffer[length] = '\0';

    /*
     * Remove line endings sent by terminal programs.
     */
    while ((length > 0U) &&
           ((rx_buffer[length - 1U] == '\r') ||
            (rx_buffer[length - 1U] == '\n')))
    {
        rx_buffer[length - 1U] = '\0';
        length--;
    }

    CONFIGURATION_MANAGER_ProcessCommand(rx_buffer, length);
}

float CONFIGURATION_MANAGER_GetVinUndervoltageThreshold(void)
{
    return vin_uv_threshold_v;
}

static void CONFIGURATION_MANAGER_TransmitString(
    const char *response)
{
    USB_DRIVER_Transmit(
        (const uint8_t *)response,
        (uint16_t)strlen(response));
}

static void CONFIGURATION_MANAGER_ProcessCommand(
    uint8_t *data,
    uint16_t length)
{
    int written;
    float new_threshold;
    char extra_character;

    (void)length;

    if (strcmp((char *)data, "ID") == 0)
    {
        CONFIGURATION_MANAGER_TransmitString(
            AEROSENSE_DEVICE_ID);
    }
    else if (strcmp((char *)data, "VERSION") == 0)
    {
        CONFIGURATION_MANAGER_TransmitString(
            AEROSENSE_FW_VERSION);
    }
    else if (strcmp((char *)data, "HWREV") == 0)
    {
        CONFIGURATION_MANAGER_TransmitString(
            AEROSENSE_HW_REVISION);
    }
    else if (strcmp((char *)data, "STATUS") == 0)
    {
        written = snprintf(
            tx_buffer,
            sizeof(tx_buffer),
            "STATE=%u;FAULTS=%u\r\n",
            (unsigned int)SYSTEM_MANAGER_GetState(),
            (unsigned int)DIAGNOSTIC_GetActiveFaultCount());

        if ((written > 0) &&
            ((uint32_t)written < sizeof(tx_buffer)))
        {
            USB_DRIVER_Transmit(
                (const uint8_t *)tx_buffer,
                (uint16_t)written);
        }
    }
    else if (strcmp((char *)data, "GET VIN_UV_THRESHOLD") == 0)
    {
        written = snprintf(
            tx_buffer,
            sizeof(tx_buffer),
            "VIN_UV_THRESHOLD=%.2f\r\n",
            (double)vin_uv_threshold_v);

        if ((written > 0) &&
            ((uint32_t)written < sizeof(tx_buffer)))
        {
            USB_DRIVER_Transmit(
                (const uint8_t *)tx_buffer,
                (uint16_t)written);
        }
    }
    else if (strncmp(
                 (char *)data,
                 "SET VIN_UV_THRESHOLD ",
                 strlen("SET VIN_UV_THRESHOLD ")) == 0)
    {
        /*
         * The second conversion detects trailing invalid characters.
         * Valid example:
         *
         * SET VIN_UV_THRESHOLD 9.5
         */
        if (sscanf(
                (char *)data + strlen("SET VIN_UV_THRESHOLD "),
                "%f %c",
                &new_threshold,
                &extra_character) != 1)
        {
            CONFIGURATION_MANAGER_TransmitString(
                "ERROR:INVALID_VALUE\r\n");
        }
        else if ((new_threshold < VIN_UV_THRESHOLD_MIN_V) ||
                 (new_threshold > VIN_UV_THRESHOLD_MAX_V))
        {
            CONFIGURATION_MANAGER_TransmitString(
                "ERROR:INVALID_VALUE\r\n");
        }
        else
        {
            vin_uv_threshold_v = new_threshold;

            CONFIGURATION_MANAGER_TransmitString(
                "OK\r\n");
        }
    }
    else
    {
        CONFIGURATION_MANAGER_TransmitString(
            "ERROR:UNKNOWN_COMMAND\r\n");
    }
}