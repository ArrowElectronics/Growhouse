/***************************************************************************//**
 * @file
 * @brief User interface specific application header file
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
