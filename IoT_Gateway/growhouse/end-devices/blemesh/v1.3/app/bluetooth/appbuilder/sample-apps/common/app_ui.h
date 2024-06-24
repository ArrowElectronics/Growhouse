/***********************************************************************************************//**
 * \file   app_ui.h
 * \brief  User interface specific application header file
 ***************************************************************************************************
 * <b> (C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

#ifndef APP_UI_H
#define APP_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/***********************************************************************************************//**
 * \defgroup app_ui Application User Interface
 * \brief User interface specific application file.
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app_ui
 * @{
 **************************************************************************************************/

/***************************************************************************************************
 *  Public Macros and Definitions
 **************************************************************************************************/
/* Text definitions*/
/* Must match length of APP_DEVNAME_DEFAULT after printf formatting */
#define APP_DEVNAME                  "BG%05u"
#define APP_DEVNAME_DEFAULT          "BG00000"
/* subtract 1 because of terminating NULL character */
#define APP_DEVNAME_LEN              (sizeof(APP_DEVNAME_DEFAULT) - 1)

/** Button press enumeration.
 *  Button press duration and button number pairs. */
typedef enum {
  /** No button press. */
  APP_UI_BTN_NONE,
  /** Button 1 short press. */
  APP_UI_BTN_0_SHORT,
  /** Button 1 medium press. */
  APP_UI_BTN_0_MED,
  /** Button 1 long press. */
  APP_UI_BTN_0_LONG,
  /** Button 1 extra long press. */
  APP_UI_BTN_0_EX_LONG,
  /** Button 2 short press. */
  APP_UI_BTN_1_SHORT,
  /** Button 2 medium press. */
  APP_UI_BTN_1_MED,
  /** Button 2 long press. */
  APP_UI_BTN_1_LONG,
  /** Button 2 extra long press. */
  APP_UI_BTN_1_EX_LONG
} AppUiBtnEvt_t;

/***************************************************************************************************
 *  Data Types
 **************************************************************************************************/

/** Button press callback. */
typedef void (*appUiBtnCback_t)(AppUiBtnEvt_t btn);

/***************************************************************************************************
 * Public Variables
 **************************************************************************************************/

/***************************************************************************************************
 * Public Function Declarations
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Register a callback function to receive button presses.
 *  \param[in]  cback  Application button callback function.
 **************************************************************************************************/
void appUiBtnRegister(appUiBtnCback_t cback);

/***********************************************************************************************//**
 *  \brief  Switch off LEDs.
 **************************************************************************************************/
void appUiLedOff(void);

/***********************************************************************************************//**
 *  \brief  Switch on LEDs with low blinking frequency.
 **************************************************************************************************/
void appUiLedLowAlert(void);

/***********************************************************************************************//**
 *  \brief  Switch on LEDs with high blinking frequency.
 **************************************************************************************************/
void appUiLedHighAlert(void);

/***********************************************************************************************//**
 *  \brief  Initialize buttons, graphics on the LCD and start repeating timer.
 *  \param[in]  devId  device ID
 **************************************************************************************************/
void appUiInit(uint16 devId);

/***********************************************************************************************//**
 *  \brief  Periodic call for User Interface specific functions.
 **************************************************************************************************/
void appUiTick(void);

/***********************************************************************************************//**
 *  \brief  Write string to graphical display.
 *  \param[in]  string  String to be displayed.
 **************************************************************************************************/
void appUiWriteString(char *string);

/** @} (end addtogroup app_ui) */
/** @} (end addtogroup Application) */

#ifdef __cplusplus
};
#endif

#endif /* APP_UI_H */
