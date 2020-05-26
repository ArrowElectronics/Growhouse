/***************************************************************************//**
 * @file
 * @brief Os BSP - Silicon Labs
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

#include  "../include/bsp_tick_rtcc.h"

#include  <common/include/lib_def.h>
#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                             BSP_TickInit()
 *
 * Description : Initialize the Board Support Package (BSP). This function is called from the start task
 *               after initializing the kernel. It should initialize the kernel ticking source.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void BSP_TickInit(void)
{
#if (!defined(OS_CFG_DYN_TICK_EN) || OS_CFG_DYN_TICK_EN == DEF_DISABLED)
  CPU_INT32U cpu_freq;
  CPU_INT32U cnts;
#endif

#if (defined(OS_CFG_DYN_TICK_EN) && OS_CFG_DYN_TICK_EN == DEF_ENABLED)
  //                                                               Init uC/OS dynamic  time src
  BSP_RTCC_TickInit();
#else
  cpu_freq = SystemCoreClockGet();                              // Determine SysTick reference freq.
  cnts = (cpu_freq / (CPU_INT32U)KAL_TickRateGet());            // Cal. SysTick counts between two OS tick interrupts.

  OS_CPU_SysTickInit(cnts);                                     // Init uC/OS periodic time src (SysTick).
#endif
}
