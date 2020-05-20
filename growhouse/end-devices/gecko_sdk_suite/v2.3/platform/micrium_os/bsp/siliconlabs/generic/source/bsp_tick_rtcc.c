/*
*********************************************************************************************************
*
*                                         Micrium OS Dynamic Tick using RTCC
*
*                                            SILICON LABS
*
* File : bsp_tick_rtcc.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include "../include/bsp_tick_rtcc.h"
#include <kernel/include/os.h>
#include "em_cmu.h"
#include "em_rtcc.h"

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  RTCC_OSCILLATOR_FREQ                     (32768u)
#define  RTCC_PRESCALER                           (1u)
#define  RTCC_CLOCK                               (cmuSelect_LFXO)
#define  BSP_OS_RTCC_TICK_RATE_HZ                 (RTCC_OSCILLATOR_FREQ / RTCC_PRESCALER)
#define  BSP_OS_RTCCTICK_TO_OSTICK(rtcctick)      (((rtcctick) * OSCfg_TickRate_Hz) / BSP_OS_RTCC_TICK_RATE_HZ)
#define  BSP_OS_OSTICK_TO_RTCCTICK(ostick)        (((ostick) * BSP_OS_RTCC_TICK_RATE_HZ) / OSCfg_TickRate_Hz)


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

/* Number of ticks scheduled to pass at the next RTCC event.
 * This is stored in OS Tick units. */
static OS_TICK BSP_OS_TicksToGo;

/* The number of OS ticks that have passed at the last time
 * we updated the OS time. This is stored in OS Tick units. */
static OS_TICK BSP_OS_LastTick;

/* Value of the RTCC counter at the last OS tick event. Note that
 * this value is always aligned on OS Tick's   */
static OS_TICK BSP_OS_LastRTCCTick;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static void RTCC_TickHandler(void);
static void RTCC_UpdateTicks(void);

/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_RTCC_TickInit()
*
* Description : Initialize the RTCC to enable dynamic ticking.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Tick_Init.
*
*               This function is an INTERNAL uC/OS-III function & MUST NOT be called by application
*               function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_RTCC_TickInit(void)
{
  RTCC_Init_TypeDef init = RTCC_INIT_DEFAULT;
  RTCC_CCChConf_TypeDef channel = RTCC_CH_INIT_COMPARE_DEFAULT;

  init.enable = false;
  init.presc  = (RTCC_CntPresc_TypeDef) CMU_DivToLog2(RTCC_PRESCALER);

  CMU_ClockSelectSet(cmuClock_LFE, RTCC_CLOCK);
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockEnable(cmuClock_RTCC, true);

  /* Initialize and start the RTCC */
  RTCC_Init(&init);
  RTCC_ChannelInit(2, &channel);
  RTCC_ChannelCCVSet(2, 0xFFFFFFFF);
  RTCC_IntEnable(RTCC_IEN_OF);
  RTCC_Enable(true);

  CPU_IntSrcHandlerSetKA(RTCC_IRQn + CPU_INT_EXT0, RTCC_TickHandler);
  NVIC_EnableIRQ(RTCC_IRQn);
}

/*
*********************************************************************************************************
*                                        RTCC_TickHandler()
*
* Description : Handle RTCC global interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is called from the RTCC interrupt handler.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void RTCC_TickHandler(void)
{
  if (RTCC_IntGetEnabled() & RTCC_IF_OF) {
    RTCC_IntClear(RTCC_IF_OF);

    /* If an OS tick is scheduled then the BSP_OS_UpdateTicks
     * function will be called when the CC2 interrupt fires.
     * If no OS tick is scheduled then we need to update the
     * tick count here. */
    if (BSP_OS_TicksToGo == 0) {
      /* Updated OS ticks based on RTCC duration */
      RTCC_UpdateTicks();
    }
  }

  if (RTCC_IntGetEnabled() & RTCC_IF_CC2) {
    RTCC_IntClear(RTCC_IF_CC2);
    EFM_ASSERT(BSP_OS_TicksToGo != 0);
    /* Updated OS ticks based on RTCC duration */
    RTCC_UpdateTicks();
    BSP_OS_TicksToGo = 0;

    RTCC_IntDisable(RTCC_IF_CC2);
    RTCC_ChannelCCVSet(2, DEF_INT_32U_MAX_VAL);
  }
}


/*
*********************************************************************************************************
*                                            BSP_OS_TickGet()
*
* Description : Get the OS Tick Counter as if it was running continuously.
*
* Argument(s) : none.
*
* Return(s)   : The effective OS Tick Counter.
*
* Caller(s)   : OS_TaskBlock, OS_TickListInsertDly and OSTimeGet.
*
*               This function is an INTERNAL uC/OS-III function & MUST NOT be called by application
*               function(s).
*
* Note(s)     : The RTCC is running continuously, however we cannot use the RTCC
*               directly as the timebase because the RTCC is wrapping much faster
*               than the OS tick.
*********************************************************************************************************
*/
OS_TICK BSP_OS_TickGet(void)
{
  OS_TICK  tick = 0;
  OS_TICK  prev;
  OS_TICK  diff;

  /* Read the tick count repeatedly until two consecutive reads are equal.
   * This is done to handle the case when the RTCC wraps */
  do
  {
    prev = tick;
    diff = RTCC_CounterGet() - BSP_OS_LastRTCCTick;
    tick = BSP_OS_LastTick + BSP_OS_RTCCTICK_TO_OSTICK(diff);
  } while (tick != prev);

  return tick;
}


/*
*********************************************************************************************************
*                                          BSP_OS_TickNextSet()
*
* Description : Set the number of OS Ticks to wait before calling OSTimeDynTick.
*
* Argument(s) : ticks       number of OS Ticks to wait.
*
* Return(s)   : Number of effective OS Ticks until next OSTimeDynTick.
*
* Caller(s)   : OS_TickTask and OS_TickListInsert.
*
*               This function is an INTERNAL uC/OS-III function & MUST NOT be called by application
*               function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
OS_TICK BSP_OS_TickNextSet(OS_TICK ticks)
{
  CPU_INT32U  rtccticks = 0u;
  CPU_INT32U  counter;
  CPU_INT32U  comp;
  CPU_INT32U  diff;

  CPU_SR_ALLOC();

  /* Check if OSTimeDynTick needs to be called. */
  if ((ticks != (OS_TICK) -1) && (ticks != 0)) {
    /* Disable event generation. */
    RTCC_IntDisable(RTCC_IEN_CC2);

    /* Do not count pending ticks twice. */
    CPU_CRITICAL_ENTER();
    ticks -= OSTickCtrPend;
    CPU_CRITICAL_EXIT();

    /* Convert OS Ticks to RTCC ticks.
     *
     * Note that this way of calculating ticks will make the RTCC tick
     * interrupt happen at an offset from original OS Ticks. So a tick
     * is scheduled in the future in x os tick time but it is important
     * to know that the RTCC interrupt is not aligned with an os tick.
     * To align it with an os tick you would have to adjust the RTCC
     * interrupt time down to nearest os tick.
     *
     * When an RTCC interrupt occurs you would only know that at leat
     * x os ticks have passed. */
    rtccticks = BSP_OS_OSTICK_TO_RTCCTICK(ticks);
    comp = RTCC_CounterGet() + rtccticks;
    BSP_OS_TicksToGo = ticks;

    /* Count the time that passed between calls to this function without
     * generating an RTCC interrupt. */
    diff = RTCC_CounterGet() - BSP_OS_LastRTCCTick;
    counter = BSP_OS_RTCCTICK_TO_OSTICK(diff);
    if (counter > 0u) {
      CPU_CRITICAL_ENTER();
      /* Update the OS with actual time passed based on the RTCC. */
      RTCC_UpdateTicks();
      CPU_CRITICAL_EXIT();
    }

    /* Set channel 2 compare value and enable interrupt. */
    RTCC_ChannelCCVSet(2, comp);
    RTCC_IntClear(RTCC_IFC_CC2);
    RTCC_IntEnable(RTCC_IEN_CC2);

  } else {
    /* If not, still keep track of time.
     * Overflow in (2^32-1)/BSP_OS_RTCC_TICK_RATE_HZ seconds.      */
    BSP_OS_TicksToGo = 0;
  }

  return BSP_OS_TicksToGo;
}

/*
*********************************************************************************************************
*                                          RTCC_UpdateTicks()
*
* Description : Updates the number of pending OS Ticks and the internal time reference.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : RTCC Interrupt handler and BSP_OS_TickNextSet.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void RTCC_UpdateTicks(void)
{
  /* Find time since last RTCC OS tick. */
  uint32_t rtccCnt = RTCC_CounterGet();
  uint32_t timeElapsed = rtccCnt - BSP_OS_LastRTCCTick;
  uint32_t tickElapsed = BSP_OS_RTCCTICK_TO_OSTICK(timeElapsed);
  if (tickElapsed > 0u) {
    /* Tell the OS how many ticks that have passed */
    OSTimeDynTick(tickElapsed);

    /* Calculate the last point in RTCC time when there should have been an OS tick */
    BSP_OS_LastTick += tickElapsed;
    uint32_t rtccDiff = rtccCnt % BSP_OS_OSTICK_TO_RTCCTICK(1);
    BSP_OS_LastRTCCTick = rtccCnt - rtccDiff;
  }
}

#endif // (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
