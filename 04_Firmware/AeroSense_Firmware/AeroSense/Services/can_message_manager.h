#ifndef CAN_MESSAGE_MANAGER_H
#define CAN_MESSAGE_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CAN Message Manager status.
 */
typedef enum
{
    CANMSG_OK = 0,
    CANMSG_ERROR,
    CANMSG_INVALID_ARG
} CANMSG_Status_t;

/**
 * @brief Initializes the CAN Message Manager.
 *
 * @return CANMSG_OK if initialization succeeds.
 */
CANMSG_Status_t CANMSG_Init(void);

/**
 * @brief Transmits environmental data.
 *
 * CAN ID: 0x100
 * Payload:
 * Byte 0-1 : Temperature [0.01 degC]
 * Byte 2-5 : Pressure [0.01 hPa]
 * Byte 6   : Measurement validity
 * Byte 7   : Reserved
 *
 * @return CANMSG_OK if transmission succeeds.
 */
CANMSG_Status_t CANMSG_TransmitEnvironment(void);

/**
 * @brief Transmits acceleration data.
 *
 * CAN ID: 0x101
 * Payload:
 * Byte 0-1 : Acceleration X [0.001 g]
 * Byte 2-3 : Acceleration Y [0.001 g]
 * Byte 4-5 : Acceleration Z [0.001 g]
 * Byte 6   : Measurement validity
 * Byte 7   : Reserved
 *
 * @return CANMSG_OK if transmission succeeds.
 */
CANMSG_Status_t CANMSG_TransmitAcceleration(void);

/**
 * @brief Transmits input-voltage data.
 *
 * CAN ID: 0x102
 * Payload:
 * Byte 0-1 : VIN [0.001 V]
 * Byte 2   : VIN validity
 * Byte 3-7 : Reserved
 *
 * @return CANMSG_OK if transmission succeeds.
 */
CANMSG_Status_t CANMSG_TransmitPower(void);

/**
 * @brief Transmits system status and diagnostic information.
 *
 * CAN ID: 0x103
 *
 * @return CANMSG_OK if transmission succeeds.
 */
CANMSG_Status_t CANMSG_TransmitStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* CAN_MESSAGE_MANAGER_H */