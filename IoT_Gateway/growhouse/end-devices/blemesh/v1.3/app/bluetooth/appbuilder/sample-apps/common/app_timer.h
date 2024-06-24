/***********************************************************************************************//**
 * \file   app_timer.h
 * \brief  Application timer header file
 ***************************************************************************************************
 * <b> (C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

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
