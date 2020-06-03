/***************************************************************************//**
 * @file
 * @brief Provide stdio retargeting configuration parameters.
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

#ifndef __SILICON_LABS_RETARGETSERIALCONFIG_H__
#define __SILICON_LABS_RETARGETSERIALCONFIG_H__

#include "bsp.h"

/***************************************************************************//**
 *
 * When retargeting serial output the user can choose which peripheral
 * to use as the serial output device. This choice is made by configuring
 * one or more of the following defines: RETARGET_USART1,
 * RETARGET_VCOM.
 *
 * This table shows the supported configurations and the resulting serial
 * output device.
 *
 * +----------------------------------------------------------------------+
 * | Defines                            | Serial Output (Locations)       |
 * |----------------------------------------------------------------------+
 * | RETARGET_USART0                    | USART0  (Rx #2, Tx #2)          |
 * | RETARGET_USART1                    | USART1  (Rx #1, Tx #1)          |
 * | RETARGET_LEUART0                   | LEUART0 (Rx #5, Tx #5)          |
 * | RETARGET_VCOM                      | VCOM using USART1               |
 * +----------------------------------------------------------------------+
 *
 * Note that the default configuration is the same as RETARGET_USART1.
 *
 ******************************************************************************/

#if defined(RETARGET_VCOM)
#define RETARGET_USART1    /* Use USART1 by default for VCOM. */
#endif

#if defined(RETARGET_USART1)
  #define RETARGET_IRQ_NAME    USART1_IRQHandler            /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART1              /* HFPER Clock */
  #define RETARGET_IRQn        USART1_IRQn                  /* IRQ number */
  #define RETARGET_UART        USART1                       /* UART instance */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC1  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC1  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortD                    /* UART transmission port */
  #define RETARGET_TXPIN       0                            /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortD                    /* UART reception port */
  #define RETARGET_RXPIN       1                            /* UART reception pin */
  #define RETARGET_USART       1                            /* Includes em_usart.h */

#elif defined(RETARGET_USART0)
  #define RETARGET_IRQ_NAME    USART0_IRQHandler            /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART0              /* HFPER Clock */
  #define RETARGET_IRQn        USART0_IRQn                  /* IRQ number */
  #define RETARGET_UART        USART0                       /* UART instance */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC2  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC2  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortC                    /* UART transmission port */
  #define RETARGET_TXPIN       11                           /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortC                    /* UART reception port */
  #define RETARGET_RXPIN       10                           /* UART reception pin */
  #define RETARGET_USART       1                            /* Includes em_usart.h */

#elif defined(RETARGET_LEUART0)
  #define RETARGET_IRQ_NAME    LEUART0_IRQHandler            /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_LEUART0              /* HFPER Clock */
  #define RETARGET_IRQn        LEUART0_IRQn                  /* IRQ number */
  #define RETARGET_UART        LEUART0                       /* UART instance */
  #define RETARGET_TX          LEUART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          LEUART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _LEUART_ROUTELOC0_TXLOC_LOC5  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _LEUART_ROUTELOC0_RXLOC_LOC5  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortC                    /* UART transmission port */
  #define RETARGET_TXPIN       14                            /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortC                    /* UART reception port */
  #define RETARGET_RXPIN       15                            /* UART reception pin */
  #define RETARGET_LEUART      1                            /* Includes em_leuart.h */

#else
#error "Illegal USART selection."
#endif

#if defined(RETARGET_VCOM)
  #define RETARGET_PERIPHERAL_ENABLE() \
  GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, \
                  BSP_BCC_ENABLE_PIN,  \
                  gpioModePushPull,    \
                  1);
#else
  #define RETARGET_PERIPHERAL_ENABLE()
#endif

#endif
