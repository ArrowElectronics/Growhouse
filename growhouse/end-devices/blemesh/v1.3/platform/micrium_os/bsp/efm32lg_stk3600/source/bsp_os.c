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
*                                             OS TICK BSP
*
*                                             SILICON LABS
*                                           EFM32LG-STK3600
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

#include  <common/include/rtos_path.h>
#include  <common/source/kal/kal_priv.h>
#include  <rtos_description.h>


#ifdef  RTOS_MODULE_KERNEL_AVAIL
#include  <kernel/include/os.h>
#elif  defined(RTOS_MODULE_KERNEL_OS2_AVAIL)
#include  <os.h>
#endif

#include  <bsp/include/bsp_micriumos.h>
#include  "../include/bsp_int.h"

                                                                /* Third Party Library Includes                         */
#include  <em_cmu.h>
#include  <em_rtc.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  OS_CFG_DYN_TICK_EN                                     /* Dynamic tick only available for Micrium OS V5        */
#define  OS_CFG_DYN_TICK_EN      DEF_DISABLED
#endif

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
#define  BSP_OS_LPTIMER_TICK_RATE_HZ          (16384u)
#define  BSP_OS_LPTICK_TO_OSTICK(lptick)    (((lptick) * OSCfg_TickRate_Hz)/BSP_OS_LPTIMER_TICK_RATE_HZ)
#define  BSP_OS_OSTICK_TO_LPTICK(ostick)    (((ostick) * BSP_OS_LPTIMER_TICK_RATE_HZ)/OSCfg_TickRate_Hz)
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                        LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
static  OS_TICK  BSP_OS_TicksToGo = 0u;                         /* Stored in OS Tick units.                             */
static  OS_TICK  BSP_OS_LastTick  = 0u;                         /* Stored in OS Tick units.                             */
static  OS_TICK  BSP_OS_UnTick    = 0u;                         /* Stored in OS Tick units.                             */
#endif

/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
static  void     BSP_OS_LP_TickInit   (void);
static  OS_TICK  BSP_OS_LP_TickGet    (void);
static  void     BSP_OS_LP_TickHandler(void);
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
*                                            BSP_TickInit()
*
* Description : Initialize the kernel ticking source.
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
#if (!defined(OS_CFG_DYN_TICK_EN) || OS_CFG_DYN_TICK_EN == DEF_DISABLED)
    CPU_INT32U  cpu_freq;
    CPU_INT32U  cnts;
#endif


#if (defined(OS_CFG_DYN_TICK_EN) && OS_CFG_DYN_TICK_EN == DEF_ENABLED)
    BSP_OS_LP_TickInit();
#elif (OS_CFG_TASK_TICK_EN == DEF_ENABLED)
    cpu_freq =  SystemCoreClockGet();                           /* Determine SysTick reference freq.                    */
    cnts     = (cpu_freq / (CPU_INT32U)KAL_TickRateGet());      /* Cal. SysTick counts between two OS tick interrupts.  */

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
#endif
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      OS DYNAMIC LOW POWER TICK
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)

/*
*********************************************************************************************************
*                                           BSP_OS_TickGet()
*
* Description : Get the OS Tick Counter as if it was running continuously.
*
* Argument(s) : None.
*
* Return(s)   : The effective OS Tick Counter.
*
* Note(s)     : This function is an INTERNAL Kernel function & MUST NOT be called by application
*               function(s).
*********************************************************************************************************
*/

OS_TICK  BSP_OS_TickGet (void)
{
    OS_TICK  tick;


    tick = BSP_OS_LastTick + BSP_OS_LPTICK_TO_OSTICK(BSP_OS_LP_TickGet());

    return (tick);
}


/*
*********************************************************************************************************
*                                         BSP_OS_TickNextSet()
*
* Description : Set the number of OS Ticks to wait before calling OSTimeTick.
*
* Argument(s) : ticks       number of OS Ticks to wait.
*
* Return(s)   : Number of effective OS Ticks until next OSTimeTick.
*
* Note(s)     : This function is an INTERNAL Kernel function & MUST NOT be called by application
*               function(s)..
*********************************************************************************************************
*/

OS_TICK  BSP_OS_TickNextSet (OS_TICK  ticks)
{
    OS_TICK     lpticks;

                                                                /* Check if OSTimeTick needs to be called.              */
    if (ticks != (OS_TICK)-1) {
                                                                /* Get tick count in LPTMR units.                       */
        lpticks = BSP_OS_OSTICK_TO_LPTICK(ticks);
                                                                /* Adjust for maximum value.                            */
        if (lpticks > DEF_INT_16U_MAX_VAL) {
            BSP_OS_TicksToGo = BSP_OS_LPTICK_TO_OSTICK(DEF_INT_16U_MAX_VAL);
            lpticks          = DEF_INT_16U_MAX_VAL;
        } else {
            BSP_OS_TicksToGo = ticks;
        }
                                                                /* Adjust for invalid value.                            */
        if (lpticks & 0xFFFF == 0) {
            lpticks          = DEF_INT_16U_MAX_VAL;
            BSP_OS_TicksToGo = BSP_OS_LPTICK_TO_OSTICK(DEF_INT_16U_MAX_VAL);
        }
    } else if (ticks == 0) {
        lpticks          = 1;
        BSP_OS_TicksToGo = 1;
    } else {
        lpticks          = DEF_INT_16U_MAX_VAL;
        BSP_OS_TicksToGo = 0;
    }


    RTC_Enable(false);                                          /* Stop timer.                                          */

                                                                /* Re-configure timer.                                  */

    RTC_CompareSet(0, lpticks);                                 /* Reset the RTC compare value to trigger at the        */
                                                                /* configured tick rate.                                */

    RTC_Enable(true);                                           /* Restart timer.                                       */

                                                                /* Return the the number of effective OS ticks that     */
    return (BSP_OS_TicksToGo);                                  /* will elapse before the next interrupt.               */
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP_OS_LP_TickInit()
*
* Description : Initialize the RTC timer to enable dynamic ticking.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_OS_LP_TickInit (void)
{
    RTC_Init_TypeDef  rtc_init_cfg;


    BSP_OS_TicksToGo = 0u;
    BSP_OS_LastTick  = 0u;


    CMU_ClockEnable(cmuClock_CORELE, true);                     /* Enable and select RTC oscillator.                    */

    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);           /* Select the low frequency crystal oscillator.         */

    CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_2);                 /* Set 2 times divider to reduce energy.                */

    CMU_ClockEnable(cmuClock_RTC, true);                        /* Enable clock to RTC module.                          */

    rtc_init_cfg.enable   = false;                              /* Specify the RTC initial configuration.               */
    rtc_init_cfg.debugRun = false;
    rtc_init_cfg.comp0Top = true;

    RTC_Init(&rtc_init_cfg);                                    /* Initialization of RTC.                               */

    RTC_IntDisable(RTC_IFC_COMP0);                              /* Disable interrupt generation from RTC0.              */

    NVIC_SetPriority(RTC_IRQn, 255);                            /* Tick interrupt MUST execute at the lowest int prio.  */

    NVIC_ClearPendingIRQ(RTC_IRQn);                             /* Enable interrupts.                                   */
    CPU_INT_SRC_HANDLER_SET_KA(BSP_INT_ID_RTC, BSP_OS_LP_TickHandler);
    CPU_IntSrcEn(BSP_INT_ID_RTC);

    RTC_CompareSet(0, 128);                                     /* Set compare register.                                */

    RTC_IntClear(RTC_IFC_COMP0);                                /* Get the RTC started.                                 */
    RTC_IntEnable(RTC_IF_COMP0);
    RTC_Enable(true);
}


/*
*********************************************************************************************************
*                                         BSP_OS_LP_TickGet()
*
* Description : Gets the RTC timer state.
*
* Argument(s) : None.
*
* Return(s)   : RTC state.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  OS_TICK  BSP_OS_LP_TickGet(void)
{
    OS_TICK  ticks;


    ticks = RTC_CounterGet();

    return (ticks);
}


/*
*********************************************************************************************************
*                                       BSP_OS_LP_TickHandler()
*
* Description : Handle the RTC interrupt for the OS LP mode.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_OS_LP_TickHandler (void)
{
    RTC_IntClear(_RTC_IFC_MASK);                                /* Clear the RTC tick interrupt.                        */

    if (BSP_OS_TicksToGo != 0) {
    	RTC_CounterSet(0);
        OSTimeDynTick(BSP_OS_TicksToGo + BSP_OS_UnTick + BSP_OS_LPTICK_TO_OSTICK(RTC_CounterGet()));
        BSP_OS_LastTick += BSP_OS_TicksToGo + BSP_OS_LPTICK_TO_OSTICK(RTC_CounterGet());

        BSP_OS_TicksToGo = 0;
        BSP_OS_UnTick    = 0;
    } else {
                                                                /* No tasks need to be awakened only keep track of time */
        BSP_OS_LastTick += BSP_OS_LPTICK_TO_OSTICK(RTC_CompareGet(0));
        BSP_OS_UnTick   += BSP_OS_LPTICK_TO_OSTICK(RTC_CompareGet(0));
    }
}
#endif                                                          /* End of OS_CFG_DYN_TICK_EN                            */
