#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CAN_DRIVER_MAX_DATA_LENGTH 8U

typedef enum
{
    CAN_DRIVER_OK = 0,
    CAN_DRIVER_ERROR,
    CAN_DRIVER_INVALID_ARG
} CAN_DRIVER_Status_t;

typedef struct
{
    uint32_t id;
    uint8_t length;
    uint8_t data[CAN_DRIVER_MAX_DATA_LENGTH];
} CAN_DRIVER_Message_t;

/**
 * @brief Initializes and starts the CAN interface.
 *
 * @return CAN_DRIVER_OK if CAN started successfully.
 */
CAN_DRIVER_Status_t CAN_DRIVER_Init(void);

/**
 * @brief Transmits a Classical CAN frame.
 *
 * @param message Pointer to CAN message.
 *
 * @return CAN_DRIVER_OK if the frame was accepted for transmission.
 */
CAN_DRIVER_Status_t CAN_DRIVER_Transmit(
    const CAN_DRIVER_Message_t *message);

#ifdef __cplusplus
}
#endif

#endif /* CAN_DRIVER_H */