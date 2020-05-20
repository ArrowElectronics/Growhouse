/***************************************************************************//**
 * @file
 * @brief CPU BSP - Template
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
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/toolchains.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                          CPU_TS_TmrInit()
 *
 * Description : Initialize & start CPU timestamp timer.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 *               This function is an INTERNAL CPU module function & MUST be implemented by application/
 *               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
 *
 * Note(s)     : (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer
 *                   if either of the following CPU features is enabled :
 *
 *                   (a) CPU timestamps
 *                   (b) CPU interrupts disabled time measurements
 *
 *                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
 *                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
 *
 *               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
 *                       data type.
 *
 *                       (1) If timer has more bits, truncate timer values' higher-order bits greater
 *                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
 *
 *                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
 *                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
 *                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
 *
 *                           In other words, if timer size is not a binary-multiple of 8-bit octets
 *                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
 *                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
 *                           minimum supported word size for CPU timestamp timers is 8-bits.
 *
 *                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
 *                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
 *
 *                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
 *
 *                   (c) When applicable, timer period SHOULD be less than the typical measured time
 *                       but MUST be less than the maximum measured time; otherwise, timer resolution
 *                       inadequate to measure desired times.
 *
 *                   See also 'CPU_TS_TmrRd()  Note #2'.
 *******************************************************************************************************/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void CPU_TS_TmrInit(void)
{
  //                                                               TODO: Init timer used for CPU TimeStamps.
}
#endif

/********************************************************************************************************
 *                                           CPU_TS_TmrRd()
 *
 * Description : Get current CPU timestamp timer count value.
 *
 * Argument(s) : none.
 *
 * Return(s)   : Timestamp timer count (see Notes #2a & #2b).
 *
 * Note(s)     : (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer
 *                   if either of the following CPU features is enabled :
 *
 *                   (a) CPU timestamps
 *                   (b) CPU interrupts disabled time measurements
 *
 *                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
 *                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
 *
 *               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
 *                       data type.
 *
 *                       (1) If timer has more bits, truncate timer values' higher-order bits greater
 *                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
 *
 *                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
 *                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
 *                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
 *
 *                           In other words, if timer size is not a binary-multiple of 8-bit octets
 *                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
 *                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
 *                           minimum supported word size for CPU timestamp timers is 8-bits.
 *
 *                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
 *                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
 *
 *                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
 *
 *                       (1) If timer is a 'down' counter whose values decrease with each time count,
 *                           then the returned timer value MUST be ones-complemented.
 *
 *                   (c) (1) When applicable, the amount of time measured by CPU timestamps is
 *                           calculated by either of the following equations :
 *
 *                           (A) Time measured  =  Number timer counts  *  Timer period
 *
 *                                   where
 *
 *                                       Number timer counts     Number of timer counts measured
 *                                       Timer period            Timer's period in some units of
 *                                                                   (fractional) seconds
 *                                       Time measured           Amount of time measured, in same
 *                                                                   units of (fractional) seconds
 *                                                                   as the Timer period
 *
 *                                                  Number timer counts
 *                           (B) Time measured  =  ---------------------
 *                                                    Timer frequency
 *
 *                                   where
 *
 *                                       Number timer counts     Number of timer counts measured
 *                                       Timer frequency         Timer's frequency in some units
 *                                                                   of counts per second
 *                                       Time measured           Amount of time measured, in seconds
 *
 *                       (2) Timer period SHOULD be less than the typical measured time but MUST be less
 *                           than the maximum measured time; otherwise, timer resolution inadequate to
 *                           measure desired times.
 *
 *               (3) This function is an INTERNAL CPU module function & MUST be implemented by application/
 *                   BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
 *******************************************************************************************************/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR CPU_TS_TmrRd(void)
{
  //                                                               TODO: Retrieve current timestamps counter count.

  return (0u);
}
#endif

/********************************************************************************************************
 *                                         CPU_TSxx_to_uSec()
 *
 * Description : Convert a 32-/64-bit CPU timestamp from timer counts to microseconds.
 *
 * Argument(s) : ts_cnts   CPU timestamp (in timestamp timer counts [see Note #2aA]).
 *
 * Return(s)   : Converted CPU timestamp (in microseconds           [see Note #2aD]).
 *
 *               This function is an (optional) CPU module application programming interface (API)
 *               function which MAY be implemented by application/BSP function(s) [see Note #1] &
 *               MAY be called by application function(s).
 *
 * Note(s)     : (1) CPU_TS32_to_uSec()/CPU_TS64_to_uSec() are application/BSP functions that MAY be
 *                   optionally defined by the developer when either of the following CPU features is
 *                   enabled :
 *
 *                   (a) CPU timestamps
 *                   (b) CPU interrupts disabled time measurements
 *
 *                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
 *                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
 *
 *               (2) (a) The amount of time measured by CPU timestamps is calculated by either of
 *                       the following equations :
 *
 *                                                                        10^6 microseconds
 *                       (1) Time measured  =   Number timer counts   *  -------------------  *  Timer period
 *                                                                            1 second
 *
 *                                              Number timer counts       10^6 microseconds
 *                       (2) Time measured  =  ---------------------  *  -------------------
 *                                                Timer frequency             1 second
 *
 *                               where
 *
 *                                   (A) Number timer counts     Number of timer counts measured
 *                                   (B) Timer frequency         Timer's frequency in some units
 *                                                                   of counts per second
 *                                   (C) Timer period            Timer's period in some units of
 *                                                                   (fractional)  seconds
 *                                   (D) Time measured           Amount of time measured,
 *                                                                   in microseconds
 *
 *                   (b) Timer period SHOULD be less than the typical measured time but MUST be less
 *                       than the maximum measured time; otherwise, timer resolution inadequate to
 *                       measure desired times.
 *
 *                   (c) Specific implementations may convert any number of CPU_TS32 or CPU_TS64 bits
 *                       -- up to 32 or 64, respectively -- into microseconds.
 *
 *               (3) This function is an (optional) CPU module application programming interface (API)
 *                   function which MAY be implemented by application/BSP function(s) [see Note #1] & MAY
 *                   be called by application function(s).
 *******************************************************************************************************/

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U CPU_TS32_to_uSec(CPU_TS32 ts_cnts)
{
  PP_UNUSED_PARAM(ts_cnts);

  //                                                               TODO: Convert timestamp counts to useconds.

  return (0u);
}
#endif

#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U CPU_TS64_to_uSec(CPU_TS64 ts_cnts)
{
  //                                                               TODO: Convert timestamp counts to useconds (64 bits).

  return (0u);
}
#endif
