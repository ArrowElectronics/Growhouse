/*****************************************************************************/
/**
 * @brief User Interface rendering for DMP demo
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "hal/plugin/glib/graphics.h"
#include "em_types.h"
#include "glib.h"
#include "dmd/dmd.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include <string.h>
#include <stdio.h>
#include "rtcdriver.h"
#include "dmp-bitmaps.h"
#include "dmp-ui.h"
#include "app/framework/include/af.h"

typedef enum {
  DMP_UI_LIGHT_ON,
  DMP_UI_LIGHT_OFF
} DmpUiLightState_t;

//#define helpmenu_line1_light  "1. PB0               "
//#define helpmenu_line2_light  "Network Present      "
//#define helpmenu_line3_light  "   Short Press:Pjoin "
//#define helpmenu_line4_light  "   Long Press :Leave "
//#define helpmenu_line5_light  "No Network"
//#define helpmenu_line6_light  "   Form Network "
//#define helpmenu_line7_light  "2. PB1:Light Ctrl    "
//#define helpmenu_line1_switch "No Network           "
//#define helpmenu_line2_switch "1. PB0 : Join        "
//#define helpmenu_line3_switch "Network Present      "
//#define helpmenu_line4_switch "1. PB0 :             "
//#define helpmenu_line5_switch "Shrt Press:Light Ctrl"
//#define helpmenu_line6_switch "Long Press:Leave Nwk "
//#define helpmenu_line7_switch "2. PB1:Light Ctrl    "

#define helpmenu_line1_light  "      **HELP**       "
#define helpmenu_line2_light  "PB0 - Toggle Light   "
#define helpmenu_line3_light  "PB1 - NWK Control    "
#define helpmenu_line4_light  " No NWK : Form NWK    "
#define helpmenu_line5_light  " NWK    : Permit join"
#define helpmenu_line6_light  " Press>5s: Leave NWK "

#define helpmenu_line1_switch "      **HELP**       "
#define helpmenu_line2_switch "PB0 - Toggle Light   "
#define helpmenu_line3_switch "PB1 - NWK Control    "
#define helpmenu_line4_switch " No NWK:Join NWK     "
#define helpmenu_line5_switch " Press>5s: Leave NWK "
#define helpmenu_line6_switch "                     "

extern EmberEventControl lcdPermitJoinEventControl;
extern GLIB_Context_t glibContext;          /* Global glib context */

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static bool isBluetoothConnected = false;
static bool blockPanIdDisplay = false;
static DmpUiLightState_t lightState = DMP_UI_LIGHT_OFF;
static DmpUiLightDirection_t lightDirection = DMP_UI_DIRECTION_INVALID;
static bool helpMenuDisplayed = false;
static uint16_t dmpUiDirectDisplayStartTime = 0;
static char bleName[10] = { 0 };

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static inline void dmpUiUpdateDisplay(void)
{
  DMD_updateDisplay();
}

static void dmpUiUpdateZigbeeStatus(DmpUiZigBeeNEtworkState_t nwState,
                                    bool withDisplayUpdate)
{
  if (!helpMenuDisplayed) {
    EmberPanId panId = emberAfGetPanId();
    char tempStr[9];

    if ((panId == 0xFFFF) && (nwState != DMP_UI_JOINING) && (nwState != DMP_UI_FORMING)) {
      nwState = DMP_UI_NO_NETWORK;
    } else if (nwState == DMP_UI_STATE_UNKNOWN) {
      if (emberStackIsUp()) {
        nwState = DMP_UI_NETWORK_UP;
      } else {
        nwState = DMP_UI_NO_NETWORK;
      }
    }

    switch (nwState) {
      case DMP_UI_NO_NETWORK:
        dmpUiDirectDisplayStartTime = 0;
        sprintf(tempStr, "No Nwk  ");
        break;
      case DMP_UI_SCANNING:
        sprintf(tempStr, "Scanning");
        break;
      case DMP_UI_JOINING:
        sprintf(tempStr, "Joining");
        break;
      case DMP_UI_FORMING:
        sprintf(tempStr, "Forming");
        break;
      case DMP_UI_NETWORK_UP:
        sprintf(tempStr, "PAN:%04X", panId);
        break;
    }

    GLIB_drawString(&glibContext, tempStr,
                    strlen(tempStr) + 1, 2, glibContext.pDisplayGeometry->ySize - 10, 0);

    if (withDisplayUpdate) {
      dmpUiUpdateDisplay();
    }
  }
}

static void dmpUiDisplayLogo(void)
{
  GLIB_drawBitmap(&glibContext,
                  SILICONLABS_X_POSITION,
                  SILICONLABS_Y_POSITION,
                  SILICONLABS_BITMAP_WIDTH,
                  SILICONLABS_BITMAP_HEIGHT,
                  siliconlabsBitmap);
}

static void dmpUiDisplayZigbeeLogo(void)
{
  GLIB_drawBitmap(&glibContext,
                  ZIGBEE_X_POSITION,
                  ZIGBEE_Y_POSITION,
                  ZIGBEE_BITMAP_WIDTH,
                  ZIGBEE_BITMAP_HEIGHT,
                  zigbeeBitmap);

  if (!blockPanIdDisplay) {
    dmpUiUpdateZigbeeStatus(DMP_UI_STATE_UNKNOWN, false);
  }
}

static void dmpUiDisplayBluetoothLogo(void)
{
  if (strlen(bleName)) {
    GLIB_drawString(&glibContext, bleName,
                    strlen(bleName) + 1, 79, glibContext.pDisplayGeometry->ySize - 10, 0);
  }

  if (isBluetoothConnected) {
    GLIB_drawBitmap(&glibContext,
                    BLUETOOTH_X_POSITION,
                    BLUETOOTH_Y_POSITION,
                    BLUETOOTH_BITMAP_WIDTH,
                    BLUETOOTH_BITMAP_HEIGHT,
                    bluetoothConnectedBitmap);
  } else {
    GLIB_drawBitmap(&glibContext,
                    BLUETOOTH_X_POSITION,
                    BLUETOOTH_Y_POSITION,
                    BLUETOOTH_BITMAP_WIDTH,
                    BLUETOOTH_BITMAP_HEIGHT,
                    bluetoothBitmap);
  }
}

static void dmpUiDisplayAppName(const char *device)
{
  char appName[20];
  sprintf(appName, "DMP Demo %s", device);

  GLIB_drawString(&glibContext, appName,
                  strlen(appName) + 1, 18, SILICONLABS_BITMAP_HEIGHT + 2, 0);
}

static void dmpUiDisplayDirection(DmpUiLightDirection_t direction)
{
  if (direction == DMP_UI_DIRECTION_ZIGBEE) {
    GLIB_drawLine(&glibContext,
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 5),
                  (ZIGBEE_Y_POSITION + ZIGBEE_BITMAP_HEIGHT / 2),
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 15),
                  (ZIGBEE_Y_POSITION + ZIGBEE_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 12),
                  (ZIGBEE_Y_POSITION + (ZIGBEE_BITMAP_HEIGHT / 2) - 3),
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 15),
                  (ZIGBEE_Y_POSITION + ZIGBEE_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 12),
                  (ZIGBEE_Y_POSITION + (ZIGBEE_BITMAP_HEIGHT / 2) + 3),
                  (ZIGBEE_X_POSITION + ZIGBEE_BITMAP_WIDTH + 15),
                  (ZIGBEE_Y_POSITION + (ZIGBEE_BITMAP_HEIGHT / 2)));
  } else if ((direction == DMP_UI_DIRECTION_BLUETOOTH)
             && (!strcmp(DEVICE_TYPE, "Light"))) {
    GLIB_drawLine(&glibContext,
                  (BLUETOOTH_X_POSITION - 5),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2),
                  (BLUETOOTH_X_POSITION - 15),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  ((BLUETOOTH_X_POSITION - 15) + 3),
                  ((BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2) - 3),
                  (BLUETOOTH_X_POSITION - 15),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2));

    GLIB_drawLine(&glibContext,
                  ((BLUETOOTH_X_POSITION - 15) + 3),
                  ((BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2) + 3),
                  (BLUETOOTH_X_POSITION - 15),
                  (BLUETOOTH_Y_POSITION + BLUETOOTH_BITMAP_HEIGHT / 2));
  }

  lightDirection = direction;

  dmpUiUpdateDisplay();
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void dmpUiInit(void)
{
  GRAPHICS_Init();
  glibContext.backgroundColor = White;
  GLIB_clear(&glibContext);
}

void dmpUiLightOn(void)
{
  lightState = DMP_UI_LIGHT_ON;
  helpMenuDisplayed = false;
  GLIB_clear(&glibContext);
  GLIB_drawBitmap(&glibContext,
                  LIGHT_X_POSITION,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  lightOnBitMap);
  dmpUiDisplayLogo();
  dmpUiDisplayAppName(DEVICE_TYPE);
  dmpUiDisplayZigbeeLogo();

  if (!strcmp(DEVICE_TYPE, "Light")) {
    dmpUiDisplayBluetoothLogo();
  }

  dmpUiUpdateDisplay();
}

void dmpUiLightOff(void)
{
  lightState = DMP_UI_LIGHT_OFF;
  helpMenuDisplayed = false;
  GLIB_clear(&glibContext);
  GLIB_drawBitmap(&glibContext,
                  LIGHT_X_POSITION,
                  LIGHT_Y_POSITION,
                  LIGHT_BITMAP_WIDTH,
                  LIGHT_BITMAP_HEIGHT,
                  lightOffBitMap);
  dmpUiDisplayLogo();
  dmpUiDisplayAppName(DEVICE_TYPE);
  dmpUiDisplayZigbeeLogo();

  if (!strcmp(DEVICE_TYPE, "Light")) {
    dmpUiDisplayBluetoothLogo();
  }

  dmpUiUpdateDisplay();
}

void dmpUiDisplayHelp(void)
{
  uint8_t y_position = SILICONLABS_BITMAP_HEIGHT + 20;

  helpMenuDisplayed = true;

  dmpUiDisplayLogo();
  dmpUiDisplayAppName(DEVICE_TYPE);
  if (!strcmp(DEVICE_TYPE, "Light")) {
    GLIB_drawString(&glibContext, helpmenu_line1_light,
                    strlen(helpmenu_line1_light) + 1, 2, y_position, 0);
    GLIB_drawString(&glibContext, helpmenu_line2_light,
                    strlen(helpmenu_line2_light) + 1, 2, y_position + 10, 0);
    GLIB_drawString(&glibContext, helpmenu_line3_light,
                    strlen(helpmenu_line3_light) + 1, 2, y_position + 20, 0);
    GLIB_drawString(&glibContext, helpmenu_line4_light,
                    strlen(helpmenu_line4_light) + 1, 2, y_position + 30, 0);
    GLIB_drawString(&glibContext, helpmenu_line5_light,
                    strlen(helpmenu_line5_light) + 1, 2, y_position + 40, 0);
    GLIB_drawString(&glibContext, helpmenu_line6_light,
                    strlen(helpmenu_line6_light) + 1, 2, y_position + 50, 0);
  } else {
    GLIB_drawString(&glibContext, helpmenu_line1_switch,
                    strlen(helpmenu_line1_switch) + 1, 2, y_position, 0);
    GLIB_drawString(&glibContext, helpmenu_line2_switch,
                    strlen(helpmenu_line2_switch) + 1, 2, y_position + 10, 0);
    GLIB_drawString(&glibContext, helpmenu_line3_switch,
                    strlen(helpmenu_line3_switch) + 1, 2, y_position + 20, 0);
    GLIB_drawString(&glibContext, helpmenu_line4_switch,
                    strlen(helpmenu_line4_switch) + 1, 2, y_position + 30, 0);
    GLIB_drawString(&glibContext, helpmenu_line5_switch,
                    strlen(helpmenu_line5_switch) + 1, 2, y_position + 40, 0);
    GLIB_drawString(&glibContext, helpmenu_line6_switch,
                    strlen(helpmenu_line6_switch) + 1, 2, y_position + 50, 0);
  }
  dmpUiUpdateDisplay();
}

void dmpUiZigBeePjoin(uint16_t duration)
{
  blockPanIdDisplay = true;
  emberEventControlSetDelayMS(lcdPermitJoinEventControl, duration);
}

void dmpUiUpdateDirection(DmpUiLightDirection_t direction)
{
  dmpUiDisplayDirection(direction);

  dmpUiDirectDisplayStartTime = halCommonGetInt16uMillisecondTick();

  emberEventControlSetInactive(lcdPermitJoinEventControl);
  emberEventControlSetDelayMS(lcdPermitJoinEventControl, DMP_UI_PJOIN_EVENT_DURATION);
}

void dmpUiBluetoothConnected(bool connectionState)
{
  if (!strcmp(DEVICE_TYPE, "Light") && !helpMenuDisplayed) {
    isBluetoothConnected = connectionState;
    dmpUiDisplayBluetoothLogo();
    dmpUiUpdateDisplay();
  }
}

void lcdPermitJoinEventHandler(void)
{
  if (!helpMenuDisplayed) {
    static bool PanIdDisplay = false;

    dmpUiClrLcdDisplayMainScreen();

    if (PanIdDisplay) {
      PanIdDisplay = false;
      dmpUiUpdateZigbeeStatus(DMP_UI_NETWORK_UP, true);
    } else {
      PanIdDisplay = true;
    }

    if (dmpUiDirectDisplayStartTime != 0) {
      if ((halCommonGetInt16uMillisecondTick() - dmpUiDirectDisplayStartTime) < 1000) {
        dmpUiDisplayDirection(lightDirection);
      } else {
        dmpUiDirectDisplayStartTime = 0;
      }
    }

    if (emberGetPermitJoining() || (dmpUiDirectDisplayStartTime != 0)) {
      emberEventControlSetDelayMS(lcdPermitJoinEventControl, DMP_UI_PJOIN_EVENT_DURATION);
    } else {
      blockPanIdDisplay = false;
      PanIdDisplay = false;
      emberEventControlSetInactive(lcdPermitJoinEventControl);
      dmpUiUpdateZigbeeStatus(DMP_UI_STATE_UNKNOWN, true);
    }
  } else {
    emberEventControlSetDelayMS(lcdPermitJoinEventControl, DMP_UI_PJOIN_EVENT_DURATION);
  }
}

void dmpUiDisplayZigBeeState(DmpUiZigBeeNEtworkState_t nwState)
{
  if (!helpMenuDisplayed) {
    blockPanIdDisplay = true;

    dmpUiClrLcdDisplayMainScreen();

    dmpUiUpdateZigbeeStatus(nwState, true);

    blockPanIdDisplay = false;
  }
}

void dmpUiClrLcdDisplayMainScreen(void)
{
  if (lightState == DMP_UI_LIGHT_OFF) {
    dmpUiLightOff();
  } else {
    dmpUiLightOn();
  }
}

void dmpUiSetBleDeviceName(char *devName)
{
  strncpy(bleName, devName, 9);
}
