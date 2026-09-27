#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    USB_DRIVER_OK = 0,
    USB_DRIVER_ERROR,
    USB_DRIVER_BUSY,
    USB_DRIVER_INVALID_ARG
} USB_DRIVER_Status_t;

#define USB_DRIVER_RX_BUFFER_SIZE 128U

/**
 * @brief Initializes the AeroSense USB driver.
 */
USB_DRIVER_Status_t USB_DRIVER_Init(void);

/**
 * @brief Transmits data through USB CDC.
 */
USB_DRIVER_Status_t USB_DRIVER_Transmit(
    const uint8_t *data,
    uint16_t length);

/**
 * @brief Passes received USB CDC data to the AeroSense USB driver.
 *
 * Called from the CubeMX CDC receive callback.
 */
void USB_DRIVER_ReceiveCallback(
    const uint8_t *data,
    uint32_t length);

/**
 * @brief Checks whether new USB data is available.
 */
uint8_t USB_DRIVER_IsDataAvailable(void);

/**
 * @brief Copies the most recently received USB data.
 *
 * @return Number of bytes copied.
 */
uint16_t USB_DRIVER_Read(
    uint8_t *data,
    uint16_t max_length);

#ifdef __cplusplus
}
#endif

#endif /* USB_DRIVER_H */