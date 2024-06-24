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
*                                                LED BSP
*
*                                             SILICON LABS
*                                 EFR32MG 2.4GHz MESH NETWORKING KIT
*                                             SLWSTK6000A
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


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/
                                                                /* Define LED GPIO control pins                         */
#define  BSP_GPIOF_LED0_PIN                 4u
#define  BSP_GPIOF_LED1_PIN                 5u


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
                                                                /* Initialize GPIO pins for LED control                 */
    GPIO_PinModeSet(gpioPortF, BSP_GPIOF_LED0_PIN , gpioModePushPull, 0u);
    GPIO_PinModeSet(gpioPortF, BSP_GPIOF_LED1_PIN , gpioModePushPull, 0u);
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
