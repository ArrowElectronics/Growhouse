/***************************************************************************//**
 * @file
 * @brief Led BSP - Silicon Labs - Efm32Pg-Slstk3401A
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
#include <em_cmu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Define LED GPIO control pins
#define  BSP_GPIO_LEDARRAY_INIT       { { gpioPortF, 4 }, { gpioPortF, 5 } }
//                                                                 LED 0        ,  LED 1

typedef struct {
  GPIO_Port_TypeDef port;
  CPU_INT08U        pin;
} LEDPorts;
//                                                                 Create array for LED port numbers
static const LEDPorts LedArray[BSP_NUM_LEDS] = BSP_GPIO_LEDARRAY_INIT;

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
  CPU_INT08U cnt;

  //                                                               Initialize GPIO pins for LED control
  for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++ ) {
    GPIO_PinModeSet(LedArray[cnt].port,
                    LedArray[cnt].pin,
                    gpioModePushPull,
                    0);
  }
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
  CPU_INT08U cnt;

  //                                                               Control individual LEDS
  if (led < BSP_NUM_LEDS) {
    GPIO_PinOutSet(LedArray[led].port,
                   LedArray[led].pin);
  } else {                                                      // Control all user LEDs
    for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++) {
      GPIO_PinOutSet(LedArray[cnt].port,
                     LedArray[cnt].pin);
    }
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
  CPU_INT08U cnt;

  //                                                               Control individual LEDS
  if (led < BSP_NUM_LEDS) {
    GPIO_PinOutClear(LedArray[led].port,
                     LedArray[led].pin);
  } else {                                                      // Control all user LEDs
    for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++) {
      GPIO_PinOutClear(LedArray[cnt].port,
                       LedArray[cnt].pin);
    }
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
  CPU_INT08U cnt;

  //                                                               Control individual LEDS
  if (led < BSP_NUM_LEDS) {
    GPIO_PinOutToggle(LedArray[led].port,
                      LedArray[led].pin);
  } else {                                                      // Control all user LEDs
    for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++) {
      GPIO_PinOutToggle(LedArray[cnt].port,
                        LedArray[cnt].pin);
    }
  }
}
