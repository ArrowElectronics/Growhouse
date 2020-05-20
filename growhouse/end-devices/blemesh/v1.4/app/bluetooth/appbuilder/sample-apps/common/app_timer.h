/***************************************************************************//**
 * @file
 * @brief Application timer header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APP_TIMER_H
#define APP_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

/***************************************************************************************************
   Public Macros and Definitions
***************************************************************************************************/

/** Timer Frequency used. */
#define TIMER_CLK_FREQ ((uint32)32768)
/** Convert msec to timer ticks. */
#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)
/** Stop timer. */
#define TIMER_STOP 0

/** Application timer enumeration. */
typedef enum {
  /** Application UI timer.
   *  This is an auto-reload timer used for timing LED and Button events. */
  UI_TIMER = 0,
  /** Advertisement timer.
   *  This is a single-shot timer used to wait some time with advertisement stopped until changes
   *  are registered in stack and we can advertise again with a different user defined advertising
   *  message. */
  ADV_TIMER,
  /** Temperature measurement timer.
   *  This is an auto-reload timer used for timing temperature measurements. */
  TEMP_TIMER,
  /** Display Polarity Inversion Timer
  * Timer for toggling the the EXTCOMIN signal, which prevents building up a DC bias
     within the Sharp memory LCD panel */
  DISP_POL_INV_TIMER
} appTimer_t;

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */

#endif /* APP_TIMER_H */
