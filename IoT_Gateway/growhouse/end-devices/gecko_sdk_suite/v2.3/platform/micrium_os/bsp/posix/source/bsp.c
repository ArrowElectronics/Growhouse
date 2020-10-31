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
*                                                 POSIX
*
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

#include  <cpu/include/cpu.h>
#include  <bsp/include/bsp_micriumos.h>

#include  <kernel/include/os_port_sel.h>


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

void  BSP_SystemInit(void)
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
    OS_CPU_SysTickInit();
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

}
