#ifndef CFG_BOARD_H
#define CFG_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_i2c.h"
#include "stm32f0xx_hal_uart.h"

extern I2C_HandleTypeDef  hi2c1;
extern UART_HandleTypeDef huart1;

#define i2c_Sensor              (&hi2c1)

#define uart_PanelLink          (&huart1)

#define gpio_Led                GPIOA
#define gpioPin_Led             GPIO_PIN_5

#define PANEL_VERSION           0x0c

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CFG_BOARD_H
