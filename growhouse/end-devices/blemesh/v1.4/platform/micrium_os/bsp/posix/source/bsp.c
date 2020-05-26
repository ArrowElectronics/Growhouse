/***************************************************************************//**
 * @file
 * @brief BSP Module - Posix
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <bsp/siliconlabs/generic/include/bsp_os.h>

#include  <kernel/include/os_port_sel.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
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
 *******************************************************************************************************/
void BSP_SystemInit(void)
{
}

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
  OS_CPU_SysTickInit();
}

/********************************************************************************************************
 *                                            BSP_OS_Init()
 *
 * Description : Initialize the Board Support Package (BSP). This function is called from the start task
 *               after initializing the kernel.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void BSP_OS_Init(void)
{
}
