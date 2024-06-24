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
 * one or more of the following defines: RETARGET_USART4, RETARGET_LEUART0,
 * RETARGET_VCOM.
 *
 * This table shows the supported configurations and the resulting serial
 * output device.
 *
 * +----------------------------------------------------------------------+
 * | Defines                            | Serial Output (Locations)       |
 * |----------------------------------------------------------------------+
 * | None                               | USART4  (Rx #4, Tx #4)          |
 * | RETARGET_USART4                    | USART4  (Rx #4, Tx #4)          |
 * | RETARGET_LEUART0                   | LEUART0 (Rx #2, Tx #2)          |
 * | RETARGET_VCOM                      | VCOM using USART4               |
 * | RETARGET_USART4 and RETARGET_VCOM  | VCOM using USART4               |
 * | RETARGET_LEUART0 and RETARGET_VCOM | Not supported by EFM32GG11.     |
 * +----------------------------------------------------------------------+
 *
 * Note that the default configuration is the same as RETARGET_USART4.
 *
 ******************************************************************************/

#if !defined(RETARGET_USART4) \
  && !defined(RETARGET_LEUART0)
#define RETARGET_USART4    /* Use USART4 by default. */
#endif

#if defined(RETARGET_USART4)
  #define RETARGET_IRQ_NAME    USART4_RX_IRQHandler         /* UART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART4              /* HFPER Clock */
  #define RETARGET_IRQn        USART4_RX_IRQn               /* IRQ number */
  #define RETARGET_UART        USART4                       /* UART instance */
  #define RETARGET_TX          USART_Tx                     /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                     /* Set RX to USART_Rx */
  #define RETARGET_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC4  /* Location of of USART TX pin */
  #define RETARGET_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC4  /* Location of of USART RX pin */
  #define RETARGET_TXPORT      gpioPortH                    /* UART transmission port */
  #define RETARGET_TXPIN       4                            /* UART transmission pin */
  #define RETARGET_RXPORT      gpioPortH                    /* UART reception port */
  #define RETARGET_RXPIN       5                            /* UART reception pin */
  #define RETARGET_USART       1                            /* Includes em_usart.h */

#elif defined(RETARGET_LEUART0)
  #define RETARGET_IRQ_NAME    LEUART0_IRQHandler           /* LEUART IRQ Handler */
  #define RETARGET_CLK         cmuClock_LEUART0             /* HFPER Clock */
  #define RETARGET_IRQn        LEUART0_IRQn                 /* IRQ number */
  #define RETARGET_UART        LEUART0                      /* LEUART instance */
  #define RETARGET_TX          LEUART_Tx                    /* Set TX to LEUART_Tx */
  #define RETARGET_RX          LEUART_Rx                    /* Set RX to LEUART_Rx */
  #define RETARGET_TX_LOCATION _LEUART_ROUTELOC0_TXLOC_LOC2 /* Location of of LEUART TX pin */
  #define RETARGET_RX_LOCATION _LEUART_ROUTELOC0_RXLOC_LOC2 /* Location of of LEUART RX pin */
  #define RETARGET_TXPORT      gpioPortE                    /* LEUART transmission port */
  #define RETARGET_TXPIN       14                           /* LEUART transmission pin */
  #define RETARGET_RXPORT      gpioPortE                    /* LEUART reception port */
  #define RETARGET_RXPIN       15                           /* LEUART reception pin */
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

// Expect use of USART4 if RETARGET_VCOM is defined.
#if !defined(RETARGET_USART4)
#error "Unsupported serial port for VCOM."
#endif
#else
  #define RETARGET_PERIPHERAL_ENABLE()
#endif

#endif
