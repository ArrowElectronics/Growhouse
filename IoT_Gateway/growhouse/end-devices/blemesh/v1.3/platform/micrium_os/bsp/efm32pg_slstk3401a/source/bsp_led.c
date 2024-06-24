/*
*********************************************************************************************************
*                                             EXAMPLE CODE
*********************************************************************************************************
* Licensing:
*   The licensor of this EXAMPLE CODE is Silicon Laboratories Inc.
*
*   Silicon Laboratories Inc. grants you a personal, worldwide, royalty-free, fully paid-up license to
*   use, copy, modify and distribute the EXAMPLE CODE software, or portions thereof, in any of your
*   products.
*
*   Your use of this EXAMPLE CODE is at your own risk. This EXAMPLE CODE does not come with any
*   warranties, and the licensor disclaims all implied warranties concerning performance, accuracy,
*   non-infringement, merchantability and fitness for your application.
*
*   The EXAMPLE CODE is provided "AS IS" and does not come with any support.
*
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*
*   You can contact us at: https://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                               LED BSP
*
*                                            SILICON LABS
*                                         EFM32PG-SLSTK3401A
*
* File : bsp_led.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  "../include/bsp_led.h"

                                                                /* Third-party include files                            */
#include <em_cmu.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/
                                                                /* Define LED GPIO control pins                         */
#define  BSP_GPIO_LEDARRAY_INIT       {{gpioPortF, 4}, {gpioPortF, 5}}
                                    /*  LED 0        ,  LED 1           */

typedef  struct {
    GPIO_Port_TypeDef    port;
    CPU_INT08U           pin;
} LEDPorts;
                                                                /* Create array for LED port numbers                    */
static  const  LEDPorts  LedArray[BSP_NUM_LEDS] = BSP_GPIO_LEDARRAY_INIT;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_LED_Init()
*
* Description : Initialize the I/O for the LEDs
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Init (void)
{
    CPU_INT08U  cnt;

                                                                /* Initialize GPIO pins for LED control                 */
    for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++ ) {
        GPIO_PinModeSet(LedArray[cnt].port,
                        LedArray[cnt].pin,
                        gpioModePushPull,
                        0);
    }
}


/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turn ON any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Note(s)     : Passing "led" a value greater than  BSP_NUM_LEDS turns on all LEDs.
*********************************************************************************************************
*/

void  BSP_LED_On (CPU_INT08U  led)
{
    CPU_INT08U  cnt;

                                                                /* Control individual LEDS                              */
    if (led < BSP_NUM_LEDS) {
        GPIO_PinOutSet(LedArray[led].port,
                       LedArray[led].pin);
    } else {                                                    /* Control all user LEDs                                */
        for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++) {
            GPIO_PinOutSet(LedArray[cnt].port,
                           LedArray[cnt].pin);
        }
    }
}


/*
*********************************************************************************************************
*                                             BSP_LED_Off()
*
* Description : Turn OFF any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Note(s)     : Passing "led" a value greater than BSP_NUM_LEDS turns off all LEDs.
*********************************************************************************************************
*/

void  BSP_LED_Off (CPU_INT08U  led)
{
    CPU_INT08U  cnt;

                                                                /* Control individual LEDS                              */
    if (led < BSP_NUM_LEDS) {
        GPIO_PinOutClear(LedArray[led].port,
                         LedArray[led].pin);
    } else {                                                    /* Control all user LEDs                                */
        for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++) {
            GPIO_PinOutClear(LedArray[cnt].port,
                             LedArray[cnt].pin);
        }
    }
}


/*
*********************************************************************************************************
*                                           BSP_LED_Toggle()
*
* Description : TOGGLE any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
* Return(s)   : none.
*
* Note(s)     : Passing "led" a value greater than BSP_NUM_LEDS toggles all LEDs
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT08U  led)
{
    CPU_INT08U  cnt;

                                                                /* Control individual LEDS                              */
    if (led < BSP_NUM_LEDS) {
        GPIO_PinOutToggle(LedArray[led].port,
                          LedArray[led].pin);
    } else {                                                    /* Control all user LEDs                                */
        for (cnt = 0; cnt < BSP_NUM_LEDS; cnt++) {
            GPIO_PinOutToggle(LedArray[cnt].port,
                              LedArray[cnt].pin);
        }
    }
}
