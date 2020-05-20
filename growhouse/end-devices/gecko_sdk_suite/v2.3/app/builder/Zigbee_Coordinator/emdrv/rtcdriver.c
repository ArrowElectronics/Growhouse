/***************************************************************************//**
 * @file rtcdriver.c
 * @brief RTCDRV timer API implementation.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>(C) Copyright 2014 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <string.h>

#include "em_device.h"
#include "em_cmu.h"
#include "em_common.h"
#include "em_core.h"

#if defined(RTCC_PRESENT) && (RTCC_COUNT == 1)
#define RTCDRV_USE_RTCC
#else
#define RTCDRV_USE_RTC
#endif

#if defined(RTCDRV_USE_RTCC)
#include "em_rtcc.h"
#else
#include "em_rtc.h"
#endif

#include "rtcdriver.h"
#if defined(EMDRV_RTCDRV_SLEEPDRV_INTEGRATION)
#include "sleep.h"
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#if defined(EMDRV_RTCDRV_SLEEPDRV_INTEGRATION) \
  && !defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)   \
  &&  defined(RTCDRV_USE_RTC)
// Do not allow EM3/EM4 energy modes when the RTC is running.
#define EMODE_DYNAMIC
#endif

#if defined(EMDRV_RTCDRV_SLEEPDRV_INTEGRATION) \
  && defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)    \
  &&  defined(RTCDRV_USE_RTC)
// Always deny EM3/EM4 energy modes when wallclock is enabled.
#define EMODE_NEVER_ALLOW_EM3EM4
#endif

//
// Various #defines to enable use of both RTC and RTCC.
//
#if defined(RTCDRV_USE_RTCC)
#define TIMEDIFF(a, b)              ((a) - (b))
#define RTC_COUNTERGET()            RTCC_CounterGet()
#define RTC_COUNTER_BITS            32
#define RTC_ALL_INTS                _RTCC_IF_MASK
#define RTC_OF_INT                  RTCC_IF_OF
#define RTC_COMP_INT                RTCC_IF_CC1
#define RTC_COUNTER_MASK            (_RTCC_CNT_MASK)
#define RTC_MAX_VALUE               (_RTCC_CNT_MASK)
#define RTC_INTDISABLE(x)           RTCC_IntDisable(x)
#define RTC_INTENABLE(x)            RTCC_IntEnable(x)
#define RTC_INTCLEAR(x)             RTCC_IntClear(x)
#define RTC_INTGET()                RTCC_IntGetEnabled()
#define RTC_COUNTERRESET()          RTCC->CNT = _RTCC_CNT_RESETVALUE
#define RTC_COMPARESET(x)           RTCC_ChannelCCVSet(1, x)
#define RTC_COMPAREGET()            RTCC_ChannelCCVGet(1)
#define NVIC_CLEARPENDINGIRQ()      NVIC_ClearPendingIRQ(RTCC_IRQn)
#define NVIC_DISABLEIRQ()           NVIC_DisableIRQ(RTCC_IRQn)
#define NVIC_ENABLEIRQ()            NVIC_EnableIRQ(RTCC_IRQn)
#define RTC_ONESHOT_TICK_ADJUST     0

#else
// To get the math correct MSB of the underlying 24bit
// counter must be in the MSB position of a uint32_t datatype.
#define TIMEDIFF(a, b)              ((( (a) << 8) - ((b) << 8) ) >> 8)
#define RTC_COUNTERGET()            RTC_CounterGet()
#define RTC_COUNTER_BITS            24
#define RTC_ALL_INTS                _RTC_IF_MASK
#define RTC_OF_INT                  RTC_IF_OF
#define RTC_COMP_INT                RTC_IF_COMP0
#define RTC_COUNTER_MASK            (_RTC_CNT_MASK)
#define RTC_MAX_VALUE               (_RTC_CNT_MASK)
#define RTC_INTDISABLE(x)           RTC_IntDisable(x)
#define RTC_INTENABLE(x)            RTC_IntEnable(x)
#define RTC_INTCLEAR(x)             RTC_IntClear(x)
#define RTC_INTGET()                RTC_IntGetEnabled()
#define RTC_COUNTERRESET()          RTC_CounterReset()
#define RTC_COMPARESET(x)           RTC_CompareSet(0, (x) & _RTC_COMP0_MASK)
#define RTC_COMPAREGET()            RTC_CompareGet(0)
#define NVIC_CLEARPENDINGIRQ()      NVIC_ClearPendingIRQ(RTC_IRQn)
#define NVIC_DISABLEIRQ()           NVIC_DisableIRQ(RTC_IRQn)
#define NVIC_ENABLEIRQ()            NVIC_EnableIRQ(RTC_IRQn)
#define RTC_ONESHOT_TICK_ADJUST     1

#endif

// Maximum number of ticks per overflow period (not the maximum tick value)
#define MAX_RTC_TICK_CNT              (RTC_MAX_VALUE + 1UL)
#define RTC_CLOSE_TO_MAX_VALUE        (RTC_MAX_VALUE - 100UL)

#if defined(_EFM32_GECKO_FAMILY)
// Assume 32 kHz RTC/RTCC clock, cmuClkDiv_2 prescaler, 16 ticks per millisecond
#define RTC_DIVIDER                   (cmuClkDiv_2)
#else
// Assume 32 kHz RTC/RTCC clock, cmuClkDiv_8 prescaler, 4 ticks per millisecond
#define RTC_DIVIDER                   (cmuClkDiv_8)
#endif

#define RTC_CLOCK                     (32768U)
#define MSEC_TO_TICKS_DIVIDER         (1000U * RTC_DIVIDER)
#define MSEC_TO_TICKS_ROUNDING_FACTOR (MSEC_TO_TICKS_DIVIDER / 2)
#define MSEC_TO_TICKS(ms)             ((((uint64_t)(ms) * RTC_CLOCK)     \
                                        + MSEC_TO_TICKS_ROUNDING_FACTOR) \
                                       / MSEC_TO_TICKS_DIVIDER)

#define TICKS_TO_MSEC_ROUNDING_FACTOR (RTC_CLOCK / 2)
#define TICKS_TO_MSEC(ticks)          ((((uint64_t)(ticks)               \
                                         * RTC_DIVIDER * 1000U)          \
                                        + TICKS_TO_MSEC_ROUNDING_FACTOR) \
                                       / RTC_CLOCK)

#define TICKS_TO_SEC_ROUNDING_FACTOR  (RTC_CLOCK / 2)
#define TICKS_TO_SEC(ticks)           ((((uint64_t)(ticks)              \
                                         * RTC_DIVIDER)                 \
                                        + TICKS_TO_SEC_ROUNDING_FACTOR) \
                                       / RTC_CLOCK)
#define TICK_TIME_USEC                (1000000 * RTC_DIVIDER / RTC_CLOCK)

typedef struct Timer{
  uint64_t            remaining;
  uint64_t            ticks;
  int                 periodicCompensationUsec;
  unsigned int        periodicDriftUsec;
  RTCDRV_Callback_t   callback;
  bool                running;
  bool                doCallback;
  bool                allocated;
  RTCDRV_TimerType_t  timerType;
  void                *user;
} Timer_t;

static Timer_t            timer[EMDRV_RTCDRV_NUM_TIMERS];
static uint32_t           lastStart;
static volatile uint32_t  startTimerNestingLevel;
static bool               inTimerIRQ;
static bool               rtcRunning;
static bool               rtcdrvIsInitialized = false;
#if defined(EMODE_DYNAMIC)
static bool               sleepBlocked;
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
static volatile uint32_t  wallClockOverflowCnt;
static uint32_t           wallClockTimeBase;
#endif

#if defined(RTCDRV_USE_RTC)
static const RTC_Init_TypeDef initRTC =
{
  true,  // Start counting when the initialization is complete.
  false, // Disable updating RTC during debug halt.
  false  // Count to maximum to wrap around.
};

#elif defined(RTCDRV_USE_RTCC)
static RTCC_Init_TypeDef initRTCC =
{
  true,                 /* Start counting when the initialization is complete. */
  false,                /* Disable updating RTC during debug halt. */
  false,                /* Prescaler counts to maximum before wrapping around. */
  false,                /* Counter counts to maximum before wrapping around. */
  rtccCntPresc_8,       /* Set RTCC prescaler to 8. */
  rtccCntTickPresc,     /* Count according to the prescaler configuration. */
#if defined(_RTCC_CTRL_BUMODETSEN_MASK)
  false,                /* Disable storing RTCC counter value in RTCC_CCV2 upon backup mode entry. */
#endif
  false,                /* LFXO fail detection disabled. */
  rtccCntModeNormal,    /* Use RTCC in normal mode and not in calendar mode. */
  false                 /* No leap year correction. */
};

static RTCC_CCChConf_TypeDef initRTCCCompareChannel =
{
  rtccCapComChModeCompare,    /* Use compare mode. */
  rtccCompMatchOutActionPulse,/* Don't care. */
  rtccPRSCh0,                 /* PRS not used. */
  rtccInEdgeNone,             /* Capture input not used. */
  rtccCompBaseCnt,            /* Compare to the Base CNT register. */
  0,                          /* Compare mask. */
  rtccDayCompareModeMonth     /* Don't care. */
};
#endif

// Default to LFXO unless configured for LFRCO.
#if defined(EMDRV_RTCDRV_USE_LFRCO)
  #define RTCDRV_OSC cmuSelect_LFRCO
#else
  #define RTCDRV_OSC cmuSelect_LFXO
#endif

static void checkAllTimers(uint32_t timeElapsed);
static void delayTicks(uint32_t ticks);
static void executeTimerCallbacks(void);
static void rescheduleRtc(uint32_t rtcCnt);

/// @endcond

/***************************************************************************//**
 * @brief
 *    Allocate timer.
 *
 * @details
 *    Reserve a timer instance.
 *
 * @param[out] id The ID of the reserved timer.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK on success.@n
 *    @ref ECODE_EMDRV_RTCDRV_ALL_TIMERS_USED when no timers are available.@n
 *    @ref ECODE_EMDRV_RTCDRV_PARAM_ERROR if an invalid ID pointer was supplied.
 ******************************************************************************/
Ecode_t RTCDRV_AllocateTimer(RTCDRV_TimerID_t *id)
{
  CORE_DECLARE_IRQ_STATE;
  int i      = 0;
  Ecode_t retVal = 0;

  CORE_ENTER_ATOMIC();
  // Iterate through the table of the timers until the first available.
  while ( (i < EMDRV_RTCDRV_NUM_TIMERS) && (timer[i].allocated) ) {
    i++;
  }

  // Check if the end of the table is reached.
  if ( i == EMDRV_RTCDRV_NUM_TIMERS ) {
    retVal = ECODE_EMDRV_RTCDRV_ALL_TIMERS_USED;
  } else {
    // Check if a NULL pointer was passed.
    if ( id != NULL ) {
      timer[i].allocated = true;
      *id = i;
      retVal = ECODE_EMDRV_RTCDRV_OK;
    } else {
      retVal = ECODE_EMDRV_RTCDRV_PARAM_ERROR;
    }
  }
  CORE_EXIT_ATOMIC();

  return retVal;
}

/***************************************************************************//**
 * @brief
 *    Millisecond delay function.
 *
 * @param[in] ms Milliseconds to stay in the delay function.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK.
 ******************************************************************************/
Ecode_t RTCDRV_Delay(uint32_t ms)
{
  uint64_t totalTicks;

  totalTicks = MSEC_TO_TICKS(ms);

  while ( totalTicks > RTC_CLOSE_TO_MAX_VALUE ) {
    delayTicks(RTC_CLOSE_TO_MAX_VALUE);
    totalTicks -= RTC_CLOSE_TO_MAX_VALUE;
  }
  delayTicks(totalTicks);

  return ECODE_EMDRV_RTCDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Free timer.
 *
 * @details
 *    Release a reserved timer.
 *
 * @param[out] id The ID of the timer to release.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK on success.@n
 *    @ref ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID if the ID has an illegal value.
 ******************************************************************************/
Ecode_t RTCDRV_FreeTimer(RTCDRV_TimerID_t id)
{
  // Check if valid timer ID.
  if ( id >= EMDRV_RTCDRV_NUM_TIMERS ) {
    return ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID;
  }

  CORE_ATOMIC_SECTION(
    timer[id].running   = false;
    timer[id].allocated = false;
    )

  return ECODE_EMDRV_RTCDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Initialize the RTCDRV driver.
 *
 * @details
 *    Will enable all necessary clocks. Initializes internal data structures
 *    and configures the underlying hardware timer.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK.
 ******************************************************************************/
Ecode_t RTCDRV_Init(void)
{
  if ( rtcdrvIsInitialized == true ) {
    return ECODE_EMDRV_RTCDRV_OK;
  }
  rtcdrvIsInitialized = true;

  // Ensure LE modules are clocked.
  CMU_ClockEnable(cmuClock_CORELE, true);

#if defined(CMU_LFECLKEN0_RTCC)
  // Enable LFECLK in CMU (will also enable oscillator if not enabled).
  CMU_ClockSelectSet(cmuClock_LFE, RTCDRV_OSC);
#else
  // Enable LFACLK in CMU (will also enable oscillator if not enabled).
  CMU_ClockSelectSet(cmuClock_LFA, RTCDRV_OSC);
#endif

#if defined(RTCDRV_USE_RTC)
  // Set clock divider.
  CMU_ClockDivSet(cmuClock_RTC, RTC_DIVIDER);

  // Enable RTC module clock.
  CMU_ClockEnable(cmuClock_RTC, true);

  // Initialize RTC.
  RTC_Init(&initRTC);

#elif defined(RTCDRV_USE_RTCC)
  // Set clock divider.
  initRTCC.presc = (RTCC_CntPresc_TypeDef)CMU_DivToLog2(RTC_DIVIDER);

  // Enable RTCC module clock.
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Initialize RTCC.
  RTCC_Init(&initRTCC);

  // Set up compare channel.
  RTCC_ChannelInit(1, &initRTCCCompareChannel);
#endif

  // Disable RTC/RTCC interrupt generation.
  RTC_INTDISABLE(RTC_ALL_INTS);
  RTC_INTCLEAR(RTC_ALL_INTS);

  RTC_COUNTERRESET();

  // Clear and then enable RTC interrupts in NVIC.
  NVIC_CLEARPENDINGIRQ();
  NVIC_ENABLEIRQ();

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
  // Enable overflow interrupt for wallclock.
  RTC_INTENABLE(RTC_OF_INT);
#endif

  // Reset RTCDRV internal data structures/variables.
  memset(timer, 0, sizeof(timer) );
  inTimerIRQ             = false;
  rtcRunning             = false;
  startTimerNestingLevel = 0;
#if defined(EMODE_DYNAMIC)
  sleepBlocked           = false;
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
  wallClockOverflowCnt = 0;
  wallClockTimeBase    = 0;

#if defined(EMODE_NEVER_ALLOW_EM3EM4)
  // Always block EM3 and EM4 if wallclock is running.
  SLEEP_SleepBlockBegin(sleepEM3);
#endif

#endif

  return ECODE_EMDRV_RTCDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Deinitialize the RTCDRV driver.
 *
 * @details
 *    Will disable interrupts and turn off the clock to the underlying hardware
 *    timer.
 *    If the integration with the SLEEP module is enabled, it will remove any
 *    restrictions that are set on energy mode usage.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK.
 ******************************************************************************/
Ecode_t RTCDRV_DeInit(void)
{
  // Disable and clear all interrupt sources.
  NVIC_DISABLEIRQ();
  RTC_INTDISABLE(RTC_ALL_INTS);
  RTC_INTCLEAR(RTC_ALL_INTS);
  NVIC_CLEARPENDINGIRQ();

  // Disable RTC module and its clock.
#if defined(RTCDRV_USE_RTC)
  RTC_Enable(false);
  CMU_ClockEnable(cmuClock_RTC, false);
#elif defined(RTCDRV_USE_RTCC)
  RTCC_Enable(false);
  CMU_ClockEnable(cmuClock_RTCC, false);
#endif

#if defined(EMODE_NEVER_ALLOW_EM3EM4)
  // End EM3 and EM4 blocking.
  SLEEP_SleepBlockEnd(sleepEM3);
#endif

#if defined(EMODE_DYNAMIC)
  // End EM3 and EM4 blocking if a block start has been set.
  if ( sleepBlocked ) {
    SLEEP_SleepBlockEnd(sleepEM3);
  }
#endif

  // Mark the driver as uninitialized.
  rtcdrvIsInitialized = false;

  return ECODE_EMDRV_RTCDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Check if a given timer is running.
 *
 * @param[in] id The ID of the timer to query.
 *
 * @param[out] isRunning True if the timer is running. False if not running.
 *                       Only valid if return status is ECODE_EMDRV_RTCDRV_OK.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK on success.@n
 *    @ref ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID if the ID has an illegal value. @n
 *    @ref ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED if the timer is not reserved.@n
 *    @ref ECODE_EMDRV_RTCDRV_PARAM_ERROR if an invalid isRunning pointer was
 *         supplied.
 ******************************************************************************/
Ecode_t RTCDRV_IsRunning(RTCDRV_TimerID_t id, bool *isRunning)
{
  CORE_DECLARE_IRQ_STATE;

  // Check if valid timer ID.
  if ( id >= EMDRV_RTCDRV_NUM_TIMERS ) {
    return ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID;
  }

  // Check pointer validity.
  if ( isRunning == NULL ) {
    return ECODE_EMDRV_RTCDRV_PARAM_ERROR;
  }

  CORE_ENTER_ATOMIC();
  // Check if the timer is reserved.
  if ( !timer[id].allocated ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED;
  }
  *isRunning = timer[id].running;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_RTCDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start a timer.
 *
 * @note
 *    It is legal to start an already running timer.
 *
 * @param[in] id The ID of the timer to start.
 * @param[in] type Timer type, oneshot or periodic. See @ref RTCDRV_TimerType_t.
 * @param[in] timeout Timeout expressed in milliseconds. If the timeout value
 *            is 0, the callback function is immediately and
 *            the timer will not be started.
 * @param[in] callback Function to call when the timer expires. See @ref
 *            RTCDRV_Callback_t. NULL is a legal value.
 * @param[in] user Extra callback function parameter for the user application.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK on success.@n
 *    @ref ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID if the ID has an illegal value.@n
 *    @ref ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED if the timer is not reserved.
 ******************************************************************************/
Ecode_t RTCDRV_StartTimer(RTCDRV_TimerID_t id,
                          RTCDRV_TimerType_t type,
                          uint32_t timeout,
                          RTCDRV_Callback_t callback,
                          void *user)
{
  CORE_DECLARE_IRQ_STATE;

  uint32_t timeElapsed, cnt, compVal, loopCnt = 0;
  uint32_t timeToNextTimerCompletion;

  // Check if valid timer ID.
  if ( id >= EMDRV_RTCDRV_NUM_TIMERS ) {
    return ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID;
  }

  CORE_ENTER_ATOMIC();
  if ( !timer[id].allocated ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED;
  }

  if ( timeout == 0 ) {
    if ( callback != NULL ) {
      callback(id, user);
    }
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_RTCDRV_OK;
  }

  cnt = RTC_COUNTERGET();

  timer[id].callback  = callback;
  timer[id].ticks     = MSEC_TO_TICKS(timeout);
  if (rtcdrvTimerTypePeriodic == type) {
    // Calculate compensation value for periodic timers.
    timer[id].periodicCompensationUsec = 1000 * timeout
                                         - (timer[id].ticks * TICK_TIME_USEC);
    timer[id].periodicDriftUsec = TICK_TIME_USEC / 2;
  } else {
    // Compensate for the fact that CNT is normally COMP0+1 after a
    // compare match event on some devices.
    timer[id].ticks -= RTC_ONESHOT_TICK_ADJUST;
  }
  // Add one tick to compensate if RTC is close to an increment event.
  timer[id].remaining = timer[id].ticks + 1;
  timer[id].running   = true;
  timer[id].timerType = type;
  timer[id].user      = user;

  if ( inTimerIRQ == true ) {
    // Exit now, remaining processing will be done in IRQ handler.
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_RTCDRV_OK;
  }

  // StartTimer() may recurse, keep track of recursion level.
  if ( startTimerNestingLevel < UINT32_MAX ) {
    startTimerNestingLevel++;
  }

  if ( rtcRunning == false ) {
#if defined(RTCDRV_USE_RTC)
    lastStart = (cnt) & RTC_COUNTER_MASK;
#elif defined(RTCDRV_USE_RTCC)
    lastStart = cnt;
#endif

    RTC_INTCLEAR(RTC_COMP_INT);

    compVal = SL_MIN(timer[id].remaining, RTC_CLOSE_TO_MAX_VALUE);
    RTC_COMPARESET(cnt + compVal);

    // Start the timer system by enabling the compare interrupt.
    RTC_INTENABLE(RTC_COMP_INT);

#if defined(EMODE_DYNAMIC)
    // When RTC is running, EM3 or EM4 is not allowed.
    if ( sleepBlocked == false ) {
      sleepBlocked = true;
      SLEEP_SleepBlockBegin(sleepEM3);
    }
#endif

    rtcRunning = true;
  } else {
    // The timer system is running. It must stop, timers must be updated with the time
    // elapsed so far, the timer with the shortest timeout must be found, and the system restarted.
    // As StartTimer() may be called from the callbacks. This
    // processing is only done at the first nesting level.
    if ( startTimerNestingLevel == 1  ) {
      timer[id].running = false;
      // This loop is repeated if CNT is incremented while processing.
      do {
        RTC_INTDISABLE(RTC_COMP_INT);

        timeElapsed = TIMEDIFF(cnt, lastStart);
#if defined(RTCDRV_USE_RTC)
        // Compensate for the fact that CNT is normally COMP0+1 after a
        // compare match event.
        if ( timeElapsed == RTC_MAX_VALUE ) {
          timeElapsed = 0;
        }
#endif

        // Update all timers with elapsed time.
        checkAllTimers(timeElapsed);

        // Execute timer callbacks.
        executeTimerCallbacks();

        // Set timer to running only after checkAllTimers() is called once.
        if ( loopCnt == 0 ) {
          timer[id].running = true;
        }
        loopCnt++;

        // Restart RTC according to the next timeout.
        rescheduleRtc(cnt);

        cnt = RTC_COUNTERGET();
        timeElapsed = TIMEDIFF(cnt, lastStart);
        timeToNextTimerCompletion = TIMEDIFF(RTC_COMPAREGET(), lastStart);

        /* If the counter has passed the COMP(ARE) register value since
           the timers were checked, recheck the timers and
           reschedule. */
      } while ( rtcRunning && (timeElapsed > timeToNextTimerCompletion));
    }
  }

  if ( startTimerNestingLevel > 0 ) {
    startTimerNestingLevel--;
  }

  CORE_EXIT_ATOMIC();
  return ECODE_EMDRV_RTCDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Stop a given timer.
 *
 * @param[in] id The ID of the timer to stop.
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK on success.@n
 *    @ref ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID if the ID has an illegal value. @n
 *    @ref ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED if the timer is not reserved.
 ******************************************************************************/
Ecode_t RTCDRV_StopTimer(RTCDRV_TimerID_t id)
{
  CORE_DECLARE_IRQ_STATE;

  // Check if valid timer ID.
  if ( id >= EMDRV_RTCDRV_NUM_TIMERS ) {
    return ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID;
  }

  CORE_ENTER_ATOMIC();
  if ( !timer[id].allocated ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED;
  }

  timer[id].running = false;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_RTCDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Get time left before a given timer expires.
 *
 * @param[in] id The ID of the timer to query.
 *
 * @param[out] timeRemaining Time left expressed in milliseconds.
 *                        Only valid if the return status is ECODE_EMDRV_RTCDRV_OK.
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK on success.@n
 *    @ref ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID if the ID has an illegal value. @n
 *    @ref ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED if the timer is not reserved.@n
 *    @ref ECODE_EMDRV_RTCDRV_TIMER_NOT_RUNNING if the timer is not running.@n
 *    @ref ECODE_EMDRV_RTCDRV_PARAM_ERROR if an invalid timeRemaining pointer
 *         was supplied.
 ******************************************************************************/
Ecode_t RTCDRV_TimeRemaining(RTCDRV_TimerID_t id, uint32_t *timeRemaining)
{
  CORE_DECLARE_IRQ_STATE;
  uint64_t ticksLeft;
  uint32_t currentCnt, lastRtcStart;

  // Check if valid timer ID.
  if ( id >= EMDRV_RTCDRV_NUM_TIMERS ) {
    return ECODE_EMDRV_RTCDRV_ILLEGAL_TIMER_ID;
  }

  // Check pointer validity.
  if ( timeRemaining == NULL ) {
    return ECODE_EMDRV_RTCDRV_PARAM_ERROR;
  }

  CORE_ENTER_ATOMIC();
  // Check if timer is reserved.
  if ( !timer[id].allocated ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_RTCDRV_TIMER_NOT_ALLOCATED;
  }

  // Check if timer is running.
  if ( !timer[id].running ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_RTCDRV_TIMER_NOT_RUNNING;
  }

  ticksLeft    = timer[id].remaining;
  currentCnt   = RTC_COUNTERGET();
  lastRtcStart = lastStart;
  CORE_EXIT_ATOMIC();

  // Get number of RTC clock ticks elapsed since last RTC reschedule.
  currentCnt = TIMEDIFF(currentCnt, lastRtcStart);

  if ( currentCnt > ticksLeft ) {
    ticksLeft = 0;
  } else {
    ticksLeft -= currentCnt;
  }

  *timeRemaining = TICKS_TO_MSEC(ticksLeft);

  return ECODE_EMDRV_RTCDRV_OK;
}

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Get wallclock time.
 *
 * @return
 *    Seconds elapsed since RTCDRV was initialized.
 ******************************************************************************/
uint32_t RTCDRV_GetWallClock(void)
{
  return wallClockTimeBase
         + (uint32_t)TICKS_TO_SEC(RTCDRV_GetWallClockTicks32() );
}
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Get wallclock tick count as a 32bit value. At 4 ticks per millisecond,
 *    overflow occurs after approximately 12.5 days.
 *
 * @return
 *    A wallclock tick counter.
 ******************************************************************************/
uint32_t RTCDRV_GetWallClockTicks32(void)
{
  uint32_t overflows, ticks;

  /* Need to re-read data in case overflow cnt is incremented while we read. */
  do {
    overflows = wallClockOverflowCnt;
    ticks     = RTC_COUNTERGET();
  } while ( overflows != wallClockOverflowCnt );

#if (RTC_COUNTER_BITS < 32)
  return (overflows << RTC_COUNTER_BITS) + ticks;
#else
  return ticks;
#endif
}
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Get a wallclock tick count as a 64 bit value. This will never overflow.
 *
 * @return
 *    A wallclock tick counter.
 ******************************************************************************/
uint64_t RTCDRV_GetWallClockTicks64(void)
{
  uint64_t overflows, ticks;

  /* Need to re-read data if the overflow cnt is incremented while reading. */
  do {
    overflows = wallClockOverflowCnt;
    ticks     = RTC_COUNTERGET();
  } while ( overflows != wallClockOverflowCnt );

  return (overflows << RTC_COUNTER_BITS) + ticks;
}
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Set wallclock time.
 *
 * @param[in] secs A value to set (seconds).
 *
 * @return
 *    @ref ECODE_EMDRV_RTCDRV_OK
 ******************************************************************************/
Ecode_t RTCDRV_SetWallClock(uint32_t secs)
{
  wallClockTimeBase = secs - TICKS_TO_SEC(RTCDRV_GetWallClockTicks32() );
  return ECODE_EMDRV_RTCDRV_OK;
}
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Convert from milliseconds to RTC/RTCC ticks.
 *
 * @param[in] ms Millisecond value to convert.
 *
 * @return
 *    A number of ticks.
 ******************************************************************************/
uint64_t  RTCDRV_MsecsToTicks(uint32_t ms)
{
  return MSEC_TO_TICKS(ms);
}
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Convert from seconds to RTC/RTCC ticks.
 *
 * @param[in] secs Second value to convert.
 *
 * @return
 *    A number of ticks.
 ******************************************************************************/
uint64_t  RTCDRV_SecsToTicks(uint32_t secs)
{
  return MSEC_TO_TICKS(1000 * secs);
}
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Convert from RTC/RTCC ticks to milliseconds.
 *
 * @param[in] ticks Number of ticks to convert.
 *
 * @return
 *   A number of milliseconds.
 ******************************************************************************/
uint32_t  RTCDRV_TicksToMsec(uint64_t ticks)
{
  return TICKS_TO_MSEC(ticks);
}

/***************************************************************************//**
 * @brief
 *    Convert from RTC/RTCC ticks to milliseconds with 64-bit resolution.
 *
 * @param[in] ticks Number of ticks to convert.
 *
 * @return
 *   A number of milliseconds as a 64-bit value.
 ******************************************************************************/
uint64_t  RTCDRV_TicksToMsec64(uint64_t ticks)
{
  return TICKS_TO_MSEC(ticks);
}
#endif

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
/***************************************************************************//**
 * @brief
 *    Convert from RTC/RTCC ticks to seconds.
 *
 * @param[in] ticks Number of ticks to convert.
 *
 * @return
 *    A number of seconds.
 ******************************************************************************/
uint32_t  RTCDRV_TicksToSec(uint64_t ticks)
{
  return TICKS_TO_MSEC(ticks) / 1000;
}
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#if defined(RTCDRV_USE_RTC)
void RTC_IRQHandler(void)
#elif defined(RTCDRV_USE_RTCC)
void RTCC_IRQHandler(void)
#endif
{
  CORE_DECLARE_IRQ_STATE;
  uint32_t flags, timeElapsed, cnt, timeToNextTimerCompletion;

  CORE_ENTER_ATOMIC();

  // CNT will normally be COMP0+1 at this point,
  // unless IRQ latency exceeded one tick period.

  flags = RTC_INTGET();

  // Acknowledge all RTC interrupts that are enabled which aren't processed
  // below to prevent looping in the IRQ handler if these become enabled.
  RTC_INTCLEAR(flags & ~(RTC_COMP_INT | RTC_OF_INT));

  if ( flags & RTC_COMP_INT ) {
    // Stop timer system by disabling the compare IRQ.
    // Update all timers with the time elapsed, call callbacks if needed,
    // then find the timer with the shortest timeout (if any at all) and
    // reenable the compare IRQ if needed.

    inTimerIRQ = true;

    cnt = RTC_COUNTERGET();

    // This loop is repeated if CNT is incremented while processing.
    do {
      RTC_INTDISABLE(RTC_COMP_INT);

      timeElapsed = TIMEDIFF(cnt, lastStart);

      // Update all timers with elapsed time.
      checkAllTimers(timeElapsed);

      // Execute timer callbacks.
      executeTimerCallbacks();

      // Restart RTC according to next timeout.
      rescheduleRtc(cnt);

      cnt = RTC_COUNTERGET();
      timeElapsed = TIMEDIFF(cnt, lastStart);
      timeToNextTimerCompletion = TIMEDIFF(RTC_COMPAREGET(), lastStart);
      /* If the counter has passed the COMP(ARE) register value since
         the timers were checked, recheck the timers and
         reschedule. */
    } while ( rtcRunning && (timeElapsed > timeToNextTimerCompletion));
    inTimerIRQ = false;
  }

#if defined(EMDRV_RTCDRV_WALLCLOCK_CONFIG)
  if ( flags & RTC_OF_INT ) {
    RTC_INTCLEAR(RTC_OF_INT);
    wallClockOverflowCnt++;
  }
#endif

  CORE_EXIT_ATOMIC();
}

static void checkAllTimers(uint32_t timeElapsed)
{
  int i;
#if defined(EMODE_DYNAMIC)
  int numOfTimersRunning = 0;
#endif

  // Iterate through the timer table.
  // Update time remaining, check for timeout and rescheduling of periodic
  // timers, check for callbacks.

  for ( i = 0; i < EMDRV_RTCDRV_NUM_TIMERS; i++ ) {
    timer[i].doCallback = false;
    if ( timer[i].running == true ) {
#if defined(EMODE_DYNAMIC)
      numOfTimersRunning++;
#endif
      if ( timer[i].remaining > timeElapsed ) {
        timer[i].remaining -= timeElapsed;
      } else {
        if ( timer[i].timerType == rtcdrvTimerTypeOneshot ) {
          timer[i].running = false;
#if defined(EMODE_DYNAMIC)
          numOfTimersRunning--;
#endif
        } else {
          // Compensate overdue periodic timers to avoid accumlating errors.
          // Note that, in some cases, the timer can be held up more than twice the time
          // of the periodic timer. This can happen on flash erase for instance or
          // when long interrupt handlers are blocking the RTC interrupt.
          uint64_t previousTick = timeElapsed - timer[i].remaining;
          timer[i].remaining = timer[i].ticks - (previousTick % timer[i].ticks);

          if ( timer[i].periodicCompensationUsec > 0 ) {
            timer[i].periodicDriftUsec += timer[i].periodicCompensationUsec;
            if (timer[i].periodicDriftUsec >= TICK_TIME_USEC) {
              // Add a tick if the timer drift is longer than the time of
              // one tick.
              timer[i].remaining += 1;
              timer[i].periodicDriftUsec -= TICK_TIME_USEC;
            }
          } else {
            timer[i].periodicDriftUsec -= timer[i].periodicCompensationUsec;
            if (timer[i].periodicDriftUsec >= TICK_TIME_USEC) {
              // Subtract one tick if the timer drift is longer than the time
              // of one tick.
              timer[i].remaining -= 1;
              timer[i].periodicDriftUsec -= TICK_TIME_USEC;
            }
          }
        }
        if ( timer[i].callback != NULL ) {
          timer[i].doCallback = true;
        }
      }
    }
  }

#if defined(EMODE_DYNAMIC)
  // If no timers are running, remove block on EM3 and EM4 sleep modes.
  if ( (numOfTimersRunning == 0) && (sleepBlocked == true) ) {
    sleepBlocked = false;
    SLEEP_SleepBlockEnd(sleepEM3);
  }
#endif
}

static void delayTicks(uint32_t ticks)
{
  uint32_t startTime;
  volatile uint32_t now;

  if ( ticks ) {
    startTime = RTC_COUNTERGET();
    do {
      now = RTC_COUNTERGET();
    } while ( TIMEDIFF(now, startTime) < ticks );
  }
}

static void executeTimerCallbacks(void)
{
  int i;

  for ( i = 0; i < EMDRV_RTCDRV_NUM_TIMERS; i++ ) {
    if ( timer[i].doCallback ) {
      timer[i].callback(i, timer[i].user);
    }
  }
}

static void rescheduleRtc(uint32_t rtcCnt)
{
  int i;
  uint64_t min = UINT64_MAX;

  // Find the timer with shortest timeout.
  for ( i = 0; i < EMDRV_RTCDRV_NUM_TIMERS; i++ ) {
    if (    (timer[i].running   == true)
            && (timer[i].remaining <  min) ) {
      min = timer[i].remaining;
    }
  }

  rtcRunning = false;
  if ( min != UINT64_MAX ) {
    min = SL_MIN(min, RTC_CLOSE_TO_MAX_VALUE);
#if defined(RTCDRV_USE_RTC)
    if ( inTimerIRQ == false ) {
      lastStart = (rtcCnt) & RTC_COUNTER_MASK;
    } else
#endif
    {
      lastStart = rtcCnt;
    }
    RTC_INTCLEAR(RTC_COMP_INT);

    RTC_COMPARESET(rtcCnt + min);

#if defined(EMODE_DYNAMIC)
    // When RTC is running, EM3 or EM4 are not allowed.
    if ( sleepBlocked == false ) {
      sleepBlocked = true;
      SLEEP_SleepBlockBegin(sleepEM3);
    }
#endif

    rtcRunning = true;

    // Reenable compare IRQ.
    RTC_INTENABLE(RTC_COMP_INT);
  }
}
/// @endcond

/* *INDENT-OFF* */
/******** THE REST OF THE FILE IS DOCUMENTATION ONLY !**********************//**
 * @addtogroup emdrv
 * @{
 * @addtogroup RTCDRV
 * @brief Real-time Clock Driver
 * @{

   @details
   The rtcdriver.c and rtcdriver.h source files for the RTCDRV device driver library are in the
   emdrv/rtcdrv folder.

   @li @ref rtcdrv_intro
   @li @ref rtcdrv_conf
   @li @ref rtcdrv_api
   @li @ref rtcdrv_example

   @n @section rtcdrv_intro Introduction

   The RTCDRV driver uses the RTC peripheral of a device in Silicon Laboratories
   Gecko microcontroller family to provide a user-configurable number of software
   millisecond timers.
   Oneshot timers and periodic timers are supported.
   Timers will, when their timeout period has expired, call a user-supplied
   callback function.
   @note The callback function is called from within an interrupt handler with
   interrupts disabled.

   In addition to the timers, RTCDRV also offers an optional wallclock
   functionality. The wallclock keeps track of the number of seconds elapsed
   since RTCDRV initialization.

   RTCDRV resolution is 1 ms with 244 us accuracy. On the EFM32G family (classic
   Gecko), the accuracy is 61 us.
   Since RTCDRV is interrupt-driven using the default RTC interrupt priority
   level, timeout accuracy will be affected by the interrupt latency of the
   system.

   @n @section rtcdrv_conf Configuration Options

   Some properties of the RTCDRV driver are compile-time configurable. These
   properties are stored in the @ref rtcdrv_config.h file. A template for this
   file, containing default values, is in the emdrv/config folder.
   Currently the configuration options are as follows:
   @li The number of timers that RTCDRV provides.
   @li Inclusion of the wallclock functionality.
   @li Integration with the SLEEP driver.

   When the integration with the SLEEP driver is enabled, RTCDRV will
   notify the SLEEP driver which energy mode can be
   safely used.

   To configure RTCDRV, provide a custom configuration file. This is an
   example @ref rtcdrv_config.h file:
   @verbatim
#ifndef SILICON_LABS_RTCDRV_CONFIG_H
#define SILICON_LABS_RTCDRV_CONFIG_H

// Define how many timers RTCDRV provides.
#define EMDRV_RTCDRV_NUM_TIMERS     (4)

// Uncomment the following line to include the wallclock functionality.
//#define EMDRV_RTCDRV_WALLCLOCK_CONFIG

// Uncomment the following line to enable integration with the SLEEP driver.
//#define EMDRV_RTCDRV_SLEEPDRV_INTEGRATION

// Uncomment the following line to let the RTCDRV clock on LFRCO.
// The default is LFXO.
//#define EMDRV_RTCDRV_USE_LFRCO

#endif
   @endverbatim

   @n @section rtcdrv_api The API

   This section contains brief descriptions of the API functions. For
   more information about input and output parameters and return values,
   click on the hyperlinked function names. Most functions return an error
   code, @ref ECODE_EMDRV_RTCDRV_OK is returned on success,
   see @ref ecode.h and @ref rtcdriver.h for other error codes.

   The application code must include the @em rtcdriver.h header file.

   All API functions can be called from within interrupt handlers.

   @ref RTCDRV_Init(), @ref RTCDRV_DeInit() @n
    These functions initialize or deinitialize the RTCDRV driver. Typically,
    @htmlonly RTCDRV_Init() @endhtmlonly is called once in the startup code.

   @ref RTCDRV_StartTimer(), @ref RTCDRV_StopTimer() @n
    Start/Stop a timer. When a timer expires, a user-supplied callback function
    is called. A pointer to this function is passed to @htmlonly
    RTCDRV_StartTimer()@endhtmlonly. See @ref TimerCallback for details of
    the callback prototype.
    Note that it is legal to start an already started timer, which is
    restarted with the new timeout value.

   @ref RTCDRV_AllocateTimer(), @ref RTCDRV_FreeTimer() @n
    Reserve/release a timer. Many functions in the API require a timer ID as
    an input parameter. Use @htmlonly RTCDRV_AllocateTimer() @endhtmlonly to
    aquire this reference.

   @ref RTCDRV_TimeRemaining() @n
    Get time left to the timer expiration.

   @ref RTCDRV_Delay() @n
    Millisecond delay function. This is an "active wait" delay function.

   @ref RTCDRV_IsRunning() @n
    Check if a timer is running.

   @ref RTCDRV_GetWallClock(), @ref RTCDRV_SetWallClock() @n
    Get or set wallclock time.

   @ref RTCDRV_GetWallClockTicks32(), @ref RTCDRV_GetWallClockTicks64() @n
    Get wallclock time expressed as number of RTC/RTCC counter ticks, available
    both as 32bit and 64 bit values.

   @ref RTCDRV_MsecsToTicks(), @ref RTCDRV_SecsToTicks(),
   @ref RTCDRV_TicksToMsec(), @ref RTCDRV_TicksToSec() @n
    Conversion functions between seconds, milliseconds, and RTC/RTCC
    counter ticks.

   @n @anchor TimerCallback <b>The timer expiry callback function:</b> @n
   The callback function, prototyped as @ref RTCDRV_Callback_t(), is called from
   within the RTC peripheral interrupt handler on timer expiration.
   @htmlonly RTCDRV_Callback_t( RTCDRV_TimerID_t id )@endhtmlonly is called with
   the timer ID as an input parameter.

   @n <b>The timer type:</b> @n
   Timers are either oneshot or periodic.
   @li Oneshot timers run only once toward their expiration.
   @li Periodic timers will be automatically restarted when they expire.

   The timer type is an enumeration, see @ref RTCDRV_TimerType_t for details.

   @n @section rtcdrv_example Example
   @verbatim
#include "rtcdriver.h"
#include <stddef.h>

int i = 0;
RTCDRV_TimerID_t id;

void myCallback( RTCDRV_TimerID_t id, void * user )
{
  (void) user; // unused argument in this example

  i++;

  if ( i < 10 ) {
    // Restart timer
    RTCDRV_StartTimer( id, rtcdrvTimerTypeOneshot, 100, myCallback, NULL );
  }
}

int main( void )
{
  // Initialization of the RTCDRV driver.
  RTCDRV_Init();

  // Reserve a timer.
  RTCDRV_AllocateTimer( &id );

  // Start a oneshot timer with 100 millisecond timeout.
  RTCDRV_StartTimer( id, rtcdrvTimerTypeOneshot, 100, myCallback, NULL );

  return 0;
}
   @endverbatim

 * @} end group RTCDRV ********************************************************
 * @} end group emdrv ****************************************************/
