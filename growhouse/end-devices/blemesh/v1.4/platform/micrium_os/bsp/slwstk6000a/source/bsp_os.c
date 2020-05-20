/***************************************************************************//**
 * @file
 * @brief Os Tick BSP - Efr32Mg 2.4Ghz Mesh Networking Kit - Slwstk6000A
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

#include  <common/include/rtos_path.h>
#include  <common/source/kal/kal_priv.h>
#include  <rtos_description.h>

#ifdef  RTOS_MODULE_KERNEL_AVAIL
#include  <kernel/include/os.h>
#elif  defined(RTOS_MODULE_KERNEL_OS2_AVAIL)
#include  <os.h>
#endif

#include  <bsp/siliconlabs/generic/include/bsp_os.h>

//                                                                 Third Party Library Includes
#include  <em_cmu.h>
#include  <em_rtcc.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  OS_CFG_DYN_TICK_EN                                     // Dynamic tick only available for Micrium OS V5
#define  OS_CFG_DYN_TICK_EN      DEF_DISABLED
#endif

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
#define  BSP_OS_LPTIMER_TICK_RATE_HZ          (16384u)
#define  BSP_OS_LPTICK_TO_OSTICK(lptick)    (((lptick) * OSCfg_TickRate_Hz) / BSP_OS_LPTIMER_TICK_RATE_HZ)
#define  BSP_OS_OSTICK_TO_LPTICK(ostick)    (((ostick) * BSP_OS_LPTIMER_TICK_RATE_HZ) / OSCfg_TickRate_Hz)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
static OS_TICK BSP_OS_TicksToGo = 0u;                           // Stored in OS Tick units.
static OS_TICK BSP_OS_LastTick = 0u;                            // Stored in OS Tick units.
static OS_TICK BSP_OS_UnTick = 0u;                              // Stored in OS Tick units.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
static void BSP_OS_LP_TickInit(void);
static OS_TICK BSP_OS_LP_TickGet(void);
static void BSP_OS_LP_TickHandler(void);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                            BSP_TickInit()
 *
 * Description : Initialize the kernel ticking source.
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
  BSP_OS_LP_TickInit();
#elif (OS_CFG_TASK_TICK_EN == DEF_ENABLED)
  cpu_freq = SystemCoreClockGet();                              // Determine SysTick reference freq.
  cnts = (cpu_freq / (CPU_INT32U)KAL_TickRateGet());            // Cal. SysTick counts between two OS tick interrupts.

  OS_CPU_SysTickInit(cnts);                                     // Init uC/OS periodic time src (SysTick).
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                      OS DYNAMIC LOW POWER TICK
 ********************************************************************************************************
 *******************************************************************************************************/

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)

/********************************************************************************************************
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
 *******************************************************************************************************/
OS_TICK BSP_OS_TickGet(void)
{
  OS_TICK tick;

  tick = BSP_OS_LastTick + BSP_OS_LPTICK_TO_OSTICK(BSP_OS_LP_TickGet());

  return (tick);
}

/********************************************************************************************************
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
 *******************************************************************************************************/
OS_TICK BSP_OS_TickNextSet(OS_TICK ticks)
{
  OS_TICK lpticks;

  //                                                               Check if OSTimeTick needs to be called.
  if (ticks != (OS_TICK)-1) {
    //                                                             Get tick count in LPTMR units.
    lpticks = BSP_OS_OSTICK_TO_LPTICK(ticks);
    //                                                             Adjust for maximum value.
    if (lpticks > DEF_INT_16U_MAX_VAL) {
      BSP_OS_TicksToGo = BSP_OS_LPTICK_TO_OSTICK(DEF_INT_16U_MAX_VAL);
      lpticks = DEF_INT_16U_MAX_VAL;
    } else {
      BSP_OS_TicksToGo = ticks;
    }
    //                                                             Adjust for invalid value.
    if (lpticks & 0xFFFF == 0) {
      lpticks = DEF_INT_16U_MAX_VAL;
      BSP_OS_TicksToGo = BSP_OS_LPTICK_TO_OSTICK(DEF_INT_16U_MAX_VAL);
    }
  } else if (ticks == 0) {
    lpticks = 1;
    BSP_OS_TicksToGo = 1;
  } else {
    lpticks = DEF_INT_16U_MAX_VAL;
    BSP_OS_TicksToGo = 0;
  }

  RTCC_Enable(false);                                           // Stop timer.

  //                                                               Re-configure timer.

  RTCC_ChannelCCVSet(0, lpticks);                               // Reset the RTC compare value to trigger at the
                                                                // configured tick rate.

  RTCC_Enable(true);                                            // Restart timer.

  //                                                               Return the the number of effective OS ticks that
  return (BSP_OS_TicksToGo);                                    // will elapse before the next interrupt.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                         BSP_OS_LP_TickInit()
 *
 * Description : Initialize the RTC timer to enable dynamic ticking.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_OS_LP_TickInit(void)
{
  RTCC_Init_TypeDef     rtcc_init_cfg;
  RTCC_CCChConf_TypeDef rtcc_ch_cfg;

  BSP_OS_TicksToGo = 0u;
  BSP_OS_LastTick = 0u;

  CMU_ClockEnable(cmuClock_CORELE, true);                       // Enable and select RTC oscillator.

  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);             // Select the low frequency crystal oscillator.

  CMU_ClockEnable(cmuClock_RTCC, true);                         // Enable clock to RTC module.

  //                                                               Specify the RTCC initial configuration.
  rtcc_init_cfg.enable = false;                                 // Don't start counting when init. is completed.
  rtcc_init_cfg.debugRun = false;                               // Disable counter during debug halt.
  rtcc_init_cfg.precntWrapOnCCV0 = false;                       // Don't care.
  rtcc_init_cfg.cntWrapOnCCV1 = true;                           // Enable counter wrap on ch1. CCV value.
  rtcc_init_cfg.presc = rtccCntPresc_2;
  rtcc_init_cfg.prescMode = rtccCntTickPresc;                   // Count using the clock input directly.
  rtcc_init_cfg.enaOSCFailDetect = false;                       // Oscillator fail detection disabled.
  rtcc_init_cfg.cntMode = rtccCntModeNormal;                    // The counter is incremented by 1 for each tick.
  rtcc_init_cfg.disLeapYearCorr = false;                        // Don't care.

  rtcc_ch_cfg.chMode = rtccCapComChModeCompare;                 // Use compare mode.
  rtcc_ch_cfg.compMatchOutAction = rtccCompMatchOutActionPulse; // Don't care.
  rtcc_ch_cfg.prsSel = rtccPRSCh0;                              // PRS not used.
  rtcc_ch_cfg.inputEdgeSel = rtccInEdgeNone;                    // Capture input not used.
  rtcc_ch_cfg.compBase = rtccCompBaseCnt;                       // Compare with Base CNT register.
  rtcc_ch_cfg.compMask = 0;                                     // Compare mask.
  rtcc_ch_cfg.dayCompMode = rtccDayCompareModeMonth;            // Don't care.

  RTCC_Init(&rtcc_init_cfg);                                    // Initialization of RTCC.
  RTCC_ChannelInit(0u, &rtcc_ch_cfg);

  RTCC_IntDisable(RTCC_IFC_CC0);                                // Disable interrupt generation from RTCC_CC0.

  NVIC_SetPriority(RTCC_IRQn, 255);                             // Tick interrupt MUST execute at the lowest int prio.

  NVIC_ClearPendingIRQ(RTCC_IRQn);                              // Enable interrupts.
  //                                                               The following line is commented to allow build tests to pass. Dynamic tick does not currently work.
  //                                                              CPU_IntSrcHandlerSetKA(RTC_IRQn + 16u, BSP_OS_LP_TickHandler);
  CPU_IntSrcEn(RTC_IRQn + 16u);

  RTCC_ChannelCCVSet(0, 128);                                   // Set compare register.

  RTCC_IntClear(RTCC_IFC_CC0);                                  // Get the RTC started.
  RTCC_IntEnable(RTCC_IF_CC0);
  RTCC_Enable(true);
}

/********************************************************************************************************
 *                                         BSP_OS_LP_TickGet()
 *
 * Description : Gets the RTC timer state.
 *
 * Argument(s) : None.
 *
 * Return(s)   : RTC state.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static OS_TICK BSP_OS_LP_TickGet(void)
{
  OS_TICK ticks;

  ticks = RTCC_CounterGet();

  return (ticks);
}

/********************************************************************************************************
 *                                       BSP_OS_LP_TickHandler()
 *
 * Description : Handle the RTC interrupt for the OS LP mode.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_OS_LP_TickHandler(void)
{
  RTCC_IntClear(_RTCC_IFC_MASK);                                // Clear the RTCC tick interrupt.

  if (BSP_OS_TicksToGo != 0) {
    RTCC_CounterSet(0);
    OSTimeDynTick(BSP_OS_TicksToGo + BSP_OS_UnTick + BSP_OS_LPTICK_TO_OSTICK(RTCC_CounterGet()));
    BSP_OS_LastTick += BSP_OS_TicksToGo + BSP_OS_LPTICK_TO_OSTICK(RTCC_CounterGet());

    BSP_OS_TicksToGo = 0;
    BSP_OS_UnTick = 0;
  } else {
    //                                                             No tasks need to be awakened only keep track of time
    BSP_OS_LastTick += BSP_OS_LPTICK_TO_OSTICK(RTCC_ChannelCCVGet(0));
    BSP_OS_UnTick += BSP_OS_LPTICK_TO_OSTICK(RTCC_ChannelCCVGet(0));
  }
}
#endif // End of OS_CFG_DYN_TICK_EN
