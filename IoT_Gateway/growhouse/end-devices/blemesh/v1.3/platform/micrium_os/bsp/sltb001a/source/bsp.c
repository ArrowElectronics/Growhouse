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
*                                        BOARD SUPPORT PACKAGE
*
*                                             SILICON LABS
*                                     EFR32MG THUNDERBOARD SENSE
*                                               SLTB001A
*
* File : bsp.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  BSP_MODULE
#include  <bsp/include/bsp_micriumos.h>
#include  "../include/bsp_led.h"

#include  <common/include/lib_utils.h>
#include  <rtos_description.h>

                                                                /* Third Party Library Includes                         */
#include  <em_chip.h>
#include  <em_cmu.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_SystemInit()
*
* Description : Initialize the Board Support Package (BSP). This function is called by the startup
*               module before initializing the kernel.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_SystemInit (void)
{
    CHIP_Init();                                                /* Chip revision alignment and errata fixes             */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);            /* Select reference clock for High Freq. clock          */
    CMU_OscillatorEnable(cmuOsc_LFXO, true, false);

    CMU_ClockEnable(cmuClock_GPIO, true);                       /* Enable GPIO module clock.                            */

    BSP_LED_Init();
}


/*
*********************************************************************************************************
*                                           BSP_PeriphInit()
*
* Description : Initialize the Board Support Package (BSP). This function is called from the start task
*               after initializing the kernel.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_PeriphInit (void)
{
    /* TODO: Add initialization here.*/
}
