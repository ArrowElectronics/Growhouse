/*
*********************************************************************************************************
*                                             EXAMPLE CODE
*********************************************************************************************************
* Licensing terms:
*   This file is provided as an example on how to use Micrium products. It has not necessarily been
*   tested under every possible condition and is only offered as a reference, without any guarantee.
*
*   Please feel free to use any application code labeled as 'EXAMPLE CODE' in your application products.
*   Example code may be used as is, in whole or in part, or may be used as a reference only. This file
*   can be modified as required.
*
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*
*   You can contact us at: http://www.micrium.com
*
*   Please help us continue to provide the Embedded community with the finest software available.
*
*   Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              OS BSP
*
*                                             Silicon Labs
*
* File : bsp_os.c
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

#include  "../include/bsp_tick_rtcc.h"

#include  <common/include/lib_def.h>
#include  <common/source/kal/kal_priv.h>


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
*                                       LOCAL FUNCTION PROTOTYPES
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

void BSP_TickInit(void)
{
#if (!defined(OS_CFG_DYN_TICK_EN) || OS_CFG_DYN_TICK_EN == DEF_DISABLED)
  CPU_INT32U  cpu_freq;
  CPU_INT32U  cnts;
#endif


#if (defined(OS_CFG_DYN_TICK_EN) && OS_CFG_DYN_TICK_EN == DEF_ENABLED)
                                                              /* Init uC/OS dynamic  time src                         */
  BSP_RTCC_TickInit();
#else
  cpu_freq =  SystemCoreClockGet();                           /* Determine SysTick reference freq.                    */
  cnts     = (cpu_freq / (CPU_INT32U)KAL_TickRateGet());      /* Cal. SysTick counts between two OS tick interrupts.  */

  OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
#endif
}
