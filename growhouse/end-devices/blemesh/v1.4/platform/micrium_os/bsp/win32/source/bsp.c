/***************************************************************************//**
 * @file
 * @brief BSP Module - Win32
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

#include  <rtos_cfg.h>
#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))
#include  <net/include/net_if_ether.h>
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
#include  <io/include/sd.h>
#include  <io/include/sd_card.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))
extern const NET_IF_ETHER_HW_INFO BSP_Winpcap_Ether;
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
extern const SD_CARD_CTRLR_DRV_INFO BSP_SD_SDHC_BSP_DrvInfo;
#endif

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
#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))
  NET_CTRLR_ETHER_REG("eth0", &BSP_Winpcap_Ether);
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
  IO_SD_CARD_CTRLR_REG("sd0", &BSP_SD_SDHC_BSP_DrvInfo);
#endif
}
