/** @file hal/micro/cortexm3/em35x/em3581/micro-features.h
 *
 * @brief
 * Constants to indicate which features the em3581 has available
 *
 * THIS IS A GENERATED FILE.  DO NOT EDIT.
 *
 * <!-- Copyright 2017 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __MICRO_FEATURES_H__
#define __MICRO_FEATURES_H__

#define CORTEXM3_EM35X_GEN4 1
#define CORTEXM3_EM358X 1

// Masks of which GPIO this chip has on which ports
#define EMBER_MICRO_PORT_A_GPIO 0xFF // 7 6 5 4 3 2 1 0
#define EMBER_MICRO_PORT_B_GPIO 0xFF // 7 6 5 4 3 2 1 0
#define EMBER_MICRO_PORT_C_GPIO 0xFF // 7 6 5 4 3 2 1 0

// Which physical serial ports are available
#define EMBER_MICRO_HAS_SC1 1
#define EMBER_MICRO_HAS_SC2 1
#define EMBER_MICRO_SERIAL_CONTROLLERS_MASK 0x03

#define EMBER_MICRO_HAS_GPTIMERS 1

#define EMBER_MICRO_HAS_ADC 1

#endif // __MICRO_FEATURES_H__
