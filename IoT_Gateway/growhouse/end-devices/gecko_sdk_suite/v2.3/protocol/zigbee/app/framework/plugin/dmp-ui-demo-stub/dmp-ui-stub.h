/**************************************************************************//**
 * Copyright 2018 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef DMP_UI_STUB_H
#define DMP_UI_STUB_H

// Stubs
#define dmpUiDisplayZigBeeState(arg)
#define dmpUiZigBeePjoin(arg)
#define dmpUiLightOff()
#define dmpUiLightOn()
#define dmpUiUpdateDirection(arg)
#define dmpUiBluetoothConnected(arg)
#define dmpUiSetBleDeviceName(arg)
#define dmpUiInit()
#define dmpUiDisplayHelp()
#define DMP_UI_DIRECTION_INVALID 0
#define DMP_UI_DIRECTION_BLUETOOTH 1
#define DMP_UI_DIRECTION_SWITCH 2
#define DMP_UI_DIRECTION_ZIGBEE  3
#define DmpUiLightDirection_t  uint8_t

#endif //DMP_UI_STUB_H
