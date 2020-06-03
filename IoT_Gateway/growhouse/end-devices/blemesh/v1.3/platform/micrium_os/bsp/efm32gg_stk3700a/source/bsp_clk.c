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
*                                               CLOCK BSP
*
*                                              Silicon Labs
*                                            EFM32GG-STK3700a
*
* File : bsp_clk.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
* Note(s) : (1) The kal_priv.h file is included only to be used by this file. Its content should remain
*               private to rtos and not be used in an application's context.
*********************************************************************************************************
*********************************************************************************************************
*/
                                                                /* Private file, use should be limited. See Note #1.    */
#include  <common/source/kal/kal_priv.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_def.h>

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#include  "../include/bsp_clk.h"

#ifdef  RTOS_MODULE_KERNEL_OS2_AVAIL
#include  <os.h>
#endif
#ifdef  RTOS_MODULE_KERNEL_AVAIL
#include  <kernel/include/os.h>
#endif

                                                                /* Third Party Library Includes                         */
#include  <em_chip.h>
#include  <em_cmu.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
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
*                                             BSP_ClkInit()
*
* Description : Initialize clocks for peripherals and controllers.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_ClkInit (void)
{
                                                                /* Select reference clock for High Freq. clock          */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

    CMU_ClockEnable(cmuClock_HFPER, DEF_TRUE);                  /* Enable Peripheral clock signal                       */
    CMU_ClockEnable(cmuClock_GPIO, DEF_TRUE);                   /* Ebable GPIO clock signals (used for LEDs)            */
}
