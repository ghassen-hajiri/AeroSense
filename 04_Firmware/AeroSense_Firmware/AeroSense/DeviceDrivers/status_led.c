#include "status_led.h"
#include "main.h"

void STATUS_LED_Init(void)
{
    STATUS_LED_Off();
}

void STATUS_LED_On(void)
{
    HAL_GPIO_WritePin(
        STATUS_LED_GPIO_Port,
        STATUS_LED_Pin,
        GPIO_PIN_SET
    );
}

void STATUS_LED_Off(void)
{
    HAL_GPIO_WritePin(
        STATUS_LED_GPIO_Port,
        STATUS_LED_Pin,
        GPIO_PIN_RESET
    );
}

void STATUS_LED_Toggle(void)
{
    HAL_GPIO_TogglePin(
        STATUS_LED_GPIO_Port,
        STATUS_LED_Pin
    );
}