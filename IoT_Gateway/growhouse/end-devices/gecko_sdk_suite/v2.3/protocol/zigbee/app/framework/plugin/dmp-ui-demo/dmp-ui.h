/****************************************************************************/
/**
 * @file dmp-ui.h
 * @brief UI interface for DMP demo
 * @version 0.0.1
 ******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef DMP_UI_H
#define DMP_UI_H

/******************************************************************************/
/**
 * @addtogroup UI Interface for DMP Demo
 * @{
 *
 * DMP UI uses the underlying DMD interface and the GLIB and exposes several
 * wrapper functions to application. These functions are used to display
 * different bitmaps for the demo.
 *
 ******************************************************************************/

#define DMP_UI_PJOIN_EVENT_DURATION           (500)

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

typedef enum {
  DMP_UI_DIRECTION_BLUETOOTH,
  DMP_UI_DIRECTION_ZIGBEE,
  DMP_UI_DIRECTION_SWITCH,
  DMP_UI_DIRECTION_INVALID
}DmpUiLightDirection_t;

typedef enum {
  DMP_UI_NO_NETWORK,
  DMP_UI_SCANNING,
  DMP_UI_JOINING,
  DMP_UI_FORMING,
  DMP_UI_NETWORK_UP,
  DMP_UI_STATE_UNKNOWN
}DmpUiZigBeeNEtworkState_t;

/*******************************************************************************
 ******************************   PROTOTYPES   *********************************
 ******************************************************************************/

/**
 * @brief
 *   Initializes the GLIB and DMD interfaces.
 *
 * @param[in] void
 *
 * @return
 *      void
 */

void dmpUiInit(void);

/**
 * @brief
 *   Updates the display to light off bitmap. This function clears the display
 *   area and re-renders the LCD with the light off bitmap with other bitmaps.
 *
 * @param[in] void
 *
 * @return
 *      void
 */

void dmpUiLightOff(void);

/**
 * @brief
 *   Updates the display to light on bitmap. This function clears the display
 *   area and re-renders the LCD with the light on bitmap with other bitmaps.
 *
 * @param[in] void
 *
 * @return
 *      void
 */

void dmpUiLightOn(void);

/**
 * @brief
 *   Updates the display to show which interface toggled the light.
 *
 * @param[in] DMP_UI_DIRECTION_BLUETOOTH or DMP_UI_DIRECTION_ZIGBEE
 *
 * @return
 *      void
 */

void dmpUiUpdateDirection(DmpUiLightDirection_t direction);

/**
 * @brief
 *   Updates the display to show if the Bluetooth is connected to the mobile device.
 *
 * @param[in] bool, true if the Light is connected to mobile device, false otherwise.
 *
 * @return
 *      void
 */

void dmpUiBluetoothConnected(bool connectionState);

/**
 * @brief
 *   Updates the display with flashing PAN ID sequence to indicate that the pjoin
 * is active. The duration between the PAN ID flashes is passed by the user. The
 * flashing sequence is stopped automatically once the pjoin is disabled.
 *
 * @param[in] duration in milliseconds, between the flashes of PAN ID.
 *
 * @return
 *      void
 */

void dmpUiZigBeePjoin(uint16_t duration);

/**
 * @brief
 *   Updates the display with PAN ID, after the device joins or leaves the network.
 *
 * @param[in] bool, if the PanId display is initiated by the application, a
 * display update needs to follow the PanId display, so the application should
 * request for the display update by passing true to this function.
 *
 * @return
 *      void
 */

void dmpUiDisplayZigBeeState(DmpUiZigBeeNEtworkState_t nwState);

/**
 * @brief
 *   Updates the display with Help menu.
 *
 * @param[in] void
 *
 * @return
 *      void
 */
void dmpUiDisplayHelp(void);

/**
 * @brief
 *   Clears the LCD screen and display the main screen.
 *
 * @param[in] void
 *
 * @return
 *      void
 */
void dmpUiClrLcdDisplayMainScreen(void);

/**
 * @brief
 *   Sets the BLE device name to be displayed on the LCD.
 *
 * @param[in] BLE device name.
 *
 * @return
 *      void
 */
void dmpUiSetBleDeviceName(char *devName);

#endif //DMP_UI_H
