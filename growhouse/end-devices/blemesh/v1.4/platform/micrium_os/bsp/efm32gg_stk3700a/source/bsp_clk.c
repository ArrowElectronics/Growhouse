/***************************************************************************//**
 * @file
 * @brief Clock BSP - Silicon Labs - Efm32Gg-Stk3700A
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
 * Note(s) : (1) The kal_priv.h file is included only to be used by this file. Its content should remain
 *               private to rtos and not be used in an application's context.
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 Private file, use should be limited. See Note #1.
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

//                                                                 Third Party Library Includes
#include  <em_chip.h>
#include  <em_cmu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                             BSP_ClkInit()
 *
 * Description : Initialize clocks for peripherals and controllers.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
void BSP_ClkInit(void)
{
  //                                                               Select reference clock for High Freq. clock
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  CMU_ClockEnable(cmuClock_HFPER, DEF_TRUE);                    // Enable Peripheral clock signal
  CMU_ClockEnable(cmuClock_GPIO, DEF_TRUE);                     // Ebable GPIO clock signals (used for LEDs)
}
