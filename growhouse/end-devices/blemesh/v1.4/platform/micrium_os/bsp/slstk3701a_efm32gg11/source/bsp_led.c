/***************************************************************************//**
 * @file
 * @brief Led BSP - Silicon Labs - Slstk3701A_Efm32Gg11
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

//                                                                 Third-party include files
#include  "em_gpio.h"
#include  "em_cmu.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Define LED GPIO control pins
#define  BSP_GPIOH_RED_LED0_PIN               10u
#define  BSP_GPIOH_RED_LED1_PIN               13u

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
  CMU_ClockEnable(cmuClock_GPIO, DEF_TRUE);                     // Enable GPIO clock signal.

  //                                                               Initialize GPIO pins for LED control
  GPIO_PinModeSet(gpioPortH, BSP_GPIOH_RED_LED0_PIN, gpioModePushPull, 0u);
  GPIO_PinModeSet(gpioPortH, BSP_GPIOH_RED_LED1_PIN, gpioModePushPull, 0u);

  BSP_LED_Off(3);                                               // Clear all LEDs.
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
      GPIO_PinOutClear(gpioPortH, BSP_GPIOH_RED_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutClear(gpioPortH, BSP_GPIOH_RED_LED1_PIN);
      break;

    default:
      GPIO_PinOutClear(gpioPortH, BSP_GPIOH_RED_LED0_PIN);
      GPIO_PinOutClear(gpioPortH, BSP_GPIOH_RED_LED1_PIN);
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
      GPIO_PinOutSet(gpioPortH, BSP_GPIOH_RED_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutSet(gpioPortH, BSP_GPIOH_RED_LED1_PIN);
      break;

    default:
      GPIO_PinOutSet(gpioPortH, BSP_GPIOH_RED_LED0_PIN);
      GPIO_PinOutSet(gpioPortH, BSP_GPIOH_RED_LED1_PIN);
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
      GPIO_PinOutToggle(gpioPortH, BSP_GPIOH_RED_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutToggle(gpioPortH, BSP_GPIOH_RED_LED1_PIN);
      break;

    default:
      GPIO_PinOutToggle(gpioPortH, BSP_GPIOH_RED_LED0_PIN);
      GPIO_PinOutToggle(gpioPortH, BSP_GPIOH_RED_LED1_PIN);
      break;
  }
}
