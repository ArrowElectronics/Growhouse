/***************************************************************************//**
 * @file
 * @brief Led BSP - Silicon Labs - Efr32Mg 2.4Ghz Mesh Networking Kit - Slwstk6000A
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
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../include/bsp_led.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Define LED GPIO control pins
#define  BSP_GPIOF_LED0_PIN                 4u
#define  BSP_GPIOF_LED1_PIN                 5u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                            BSP_LED_Init()
 *
 * Description : Initialize the I/O for the LEDs
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
void BSP_LED_Init(void)
{
  //                                                               Initialize GPIO pins for LED control
  GPIO_PinModeSet(gpioPortF, BSP_GPIOF_LED0_PIN, gpioModePushPull, 0u);
  GPIO_PinModeSet(gpioPortF, BSP_GPIOF_LED1_PIN, gpioModePushPull, 0u);
}

/********************************************************************************************************
 *                                             BSP_LED_On()
 *
 * Description : Turn ON any or all the LEDs on the board.
 *
 * Argument(s) : led     The ID of the LED to control:
 *
 * Return(s)   : none.
 *
 * Note(s)     : Passing "led" a value greater than  BSP_NUM_LEDS turns on all LEDs.
 *******************************************************************************************************/
void BSP_LED_On(CPU_INT08U led)
{
  switch (led) {
    case 0:
      GPIO_PinOutSet(gpioPortF, BSP_GPIOF_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutSet(gpioPortF, BSP_GPIOF_LED1_PIN);
      break;

    default:
      GPIO_PinOutSet(gpioPortF, BSP_GPIOF_LED0_PIN);
      GPIO_PinOutSet(gpioPortF, BSP_GPIOF_LED1_PIN);
      break;
  }
}

/********************************************************************************************************
 *                                             BSP_LED_Off()
 *
 * Description : Turn OFF any or all the LEDs on the board.
 *
 * Argument(s) : led     The ID of the LED to control:
 *
 * Return(s)   : none.
 *
 * Note(s)     : Passing "led" a value greater than BSP_NUM_LEDS turns off all LEDs.
 *******************************************************************************************************/
void BSP_LED_Off(CPU_INT08U led)
{
  switch (led) {
    case 0:
      GPIO_PinOutClear(gpioPortF, BSP_GPIOF_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutClear(gpioPortF, BSP_GPIOF_LED1_PIN);
      break;

    default:
      GPIO_PinOutClear(gpioPortF, BSP_GPIOF_LED0_PIN);
      GPIO_PinOutClear(gpioPortF, BSP_GPIOF_LED1_PIN);
      break;
  }
}

/********************************************************************************************************
 *                                           BSP_LED_Toggle()
 *
 * Description : TOGGLE any or all the LEDs on the board.
 *
 * Argument(s) : led     The ID of the LED to control:
 *
 * Return(s)   : none.
 *
 * Note(s)     : Passing "led" a value greater than BSP_NUM_LEDS toggles all LEDs
 *******************************************************************************************************/
void BSP_LED_Toggle(CPU_INT08U led)
{
  switch (led) {
    case 0:
      GPIO_PinOutToggle(gpioPortF, BSP_GPIOF_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutToggle(gpioPortF, BSP_GPIOF_LED1_PIN);
      break;

    default:
      GPIO_PinOutToggle(gpioPortF, BSP_GPIOF_LED0_PIN);
      GPIO_PinOutToggle(gpioPortF, BSP_GPIOF_LED1_PIN);
      break;
  }
}
