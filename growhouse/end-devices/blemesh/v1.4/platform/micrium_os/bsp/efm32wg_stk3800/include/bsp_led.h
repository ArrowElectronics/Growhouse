/***************************************************************************//**
 * @file
 * @brief Led BSP - Silicon Labs - Efm32 Wonder Gecko Starter Kit
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _BSP_LED_H_
#define  _BSP_LED_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

//                                                                 Third-party includes
#include  "em_gpio.h"
#include  "em_cmu.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  BSP_NUM_LEDS                           2               // Total number of LEDs on Wonder Gecko Starter Kit

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void BSP_LED_Init(void);

void BSP_LED_On(CPU_INT08U led);

void BSP_LED_Off(CPU_INT08U led);

void BSP_LED_Toggle(CPU_INT08U led);

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of module include.