// *******************************************************************
// * led-rgb-pwm.h
// *
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *******************************************************************
#ifndef SILABS_LED_RGB_PWM_H
#define SILABS_LED_RGB_PWM_H

void emberAfLedRgbPwmComputeRgbFromXy(uint8_t endpoint);
void emberAfLedRgbPwmComputeRgbFromColorTemp(uint8_t endpoint);
void emberAfLedRgbComputeRgbFromHSV(uint8_t endpoint);

#endif
