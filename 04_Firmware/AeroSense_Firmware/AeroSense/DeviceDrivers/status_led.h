#ifndef STATUS_LED_H
#define STATUS_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the AeroSense status LED driver.
 *
 * The GPIO itself is configured by STM32CubeMX.
 * This function establishes the defined application startup state.
 */
void STATUS_LED_Init(void);

/**
 * @brief Turns the status LED on.
 */
void STATUS_LED_On(void);

/**
 * @brief Turns the status LED off.
 */
void STATUS_LED_Off(void);

/**
 * @brief Toggles the current status LED state.
 */
void STATUS_LED_Toggle(void);

#ifdef __cplusplus
}
#endif

#endif /* STATUS_LED_H */