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
*                                              BSP MODULE
*
*                                                 WIN32
* File : bsp.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <rtos_cfg.h>
#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))
#include  <net/include/net_if_ether.h>
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
#include  <io/include/sd.h>
#include  <io/include/sd_card.h>
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))
extern  const  NET_IF_ETHER_HW_INFO  BSP_Winpcap_Ether;
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
extern  const  SD_CARD_CTRLR_DRV_INFO BSP_SD_SDHC_BSP_DrvInfo;
#endif


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

}


/*
*********************************************************************************************************
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
*********************************************************************************************************
*/

void  BSP_TickInit (void)
{

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

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))
    NET_CTRLR_ETHER_REG("eth0", &BSP_Winpcap_Ether);
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
 IO_SD_CARD_CTRLR_REG("sd0", &BSP_SD_SDHC_BSP_DrvInfo);
#endif
}
