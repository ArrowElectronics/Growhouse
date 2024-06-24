// *******************************************************************
// * led-temp-pwm.h
// *
// * Implements the color control server for color temperature bulbs.
// * Note: This is HA certifiable and has passed HA certification for at least
// * one customer project.
// *
// * Copyright 2015 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

// Nothing here for now..
#ifndef SILABS_LED_TEMP_PWM_H
#define SILABS_LED_TEMP_PWM_H

enum {
  EMBER_ZCL_COLOR_TEMP_MOVE_MODE_STOP = 0x00,
  EMBER_ZCL_COLOR_TEMP_MOVE_MODE_UP   = 0x01,
  EMBER_ZCL_COLOR_TEMP_MOVE_MODE_DOWN = 0x03,
};

#endif
