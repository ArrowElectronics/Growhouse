/***************************************************************************//**
 * @file
 * @brief Provide BSP (board support package) configuration parameters.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __BSPCONFIG_H
#define __BSPCONFIG_H

#define BSP_STK
#define BSP_STK_BRD2200

#define BSP_BCC_USART       UART0
#define BSP_BCC_CLK         cmuClock_UART0
#define BSP_BCC_LOCATION    UART_ROUTE_LOCATION_LOC1
#define BSP_BCC_TXPORT      gpioPortE
#define BSP_BCC_TXPIN       0
#define BSP_BCC_RXPORT      gpioPortE
#define BSP_BCC_RXPIN       1
#define BSP_BCC_ENABLE_PORT gpioPortF
#define BSP_BCC_ENABLE_PIN  7

#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS          2
#define BSP_GPIO_LED0_PORT      gpioPortE
#define BSP_GPIO_LED0_PIN       2
#define BSP_GPIO_LED1_PORT      gpioPortE
#define BSP_GPIO_LED1_PIN       3
#define BSP_GPIO_LEDARRAY_INIT { { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN }, { BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN } }

#define BSP_GPIO_BUTTONS
#define BSP_NO_OF_BUTTONS       2
#define BSP_GPIO_PB0_PORT       gpioPortB
#define BSP_GPIO_PB0_PIN        9
#define BSP_GPIO_PB1_PORT       gpioPortB
#define BSP_GPIO_PB1_PIN        10

#define BSP_GPIO_BUTTONARRAY_INIT { { BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN }, { BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN } }

#define BSP_STK_USE_EBI

#define BSP_INIT_DEFAULT  0

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif
