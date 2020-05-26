/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef CSLIB_HWCONFIG_H
#define CSLIB_HWCONFIG_H

#include "em_gpio.h"
#include "em_acmp.h"

/**************************************************************************//**
 *
 * @addtogroup cslib_HWconfig CSLIB library hardware configuration
 * @{
 *
 * @brief Hardware configuration for CSLIB
 *
 * These definitions configure the capacitive sensing hardware in
 * active and sleep mode scans.  They also configure channel-by-channel
 * thresholds and expected touchd deltas.
 *
 *****************************************************************************/

/// @brief Defines the size of the sensor node array.
/// Also defines volatile arrays that have a one-to-one correspondence
/// to the number of sensors in the project.
/// @note Minimum value is 1, maximum value is the number of capacitive
/// sensing-enabled pins on the device
#define DEF_NUM_SENSORS                           2

/// @brief Cross reference between sensor number ordering and pin
/// ordering.  This allows for using pins that are not in order when
/// doing layout.
#define MUX_VALUE_ARRAY 0, 1

/// @brief Per channel active threshold setting
/// @note Minimum threshold used is @ref INACTIVE_THRESHOLD_ARRAY value,
/// maximum value is 100
#define ACTIVE_THRESHOLD_ARRAY 70, 70

/// @brief Per channel inactive threshold setting
/// @note Minimum threshold used is 1, maximum value is @ref ACTIVE_THRESHOLD_ARRAY
#define INACTIVE_THRESHOLD_ARRAY 30, 30

/// @brief Per-channel expected touch delta.  This value describes the difference
/// in capacitive sensing output codes between the inactive/baseline of the sensor,
/// and the output of the sensor when active(touched).
/// @note These values should be defined in terms of X/16, or X>>4, as they are stored
/// in a packed byte array
#define AVERAGE_TOUCH_DELTA_ARRAY 50 >> 4, 50 >> 4

/// @brief Per channel ACMP_CAPSENSE channel value for each enabled sensor
#define CSLIB_MUX_INPUT acmpInputAPORT4XCH27, acmpInputAPORT3XCH28

/// @brief Array stores ACMP input settings
extern const uint16_t CSLIB_muxInput[];

/// @brief Array of cross-references between sensor number and actual pin number
extern const uint8_t CSLIB_muxValues[];

/** @} (end cslib_HWconfig) */
#endif // CSLIB_HWCONFIG_H
