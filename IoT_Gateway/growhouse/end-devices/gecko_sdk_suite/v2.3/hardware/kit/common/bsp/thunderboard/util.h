/***************************************************************************//**
 * @file util.h
 * @brief Utility Functions for the Thunderboard Sense
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup Util
 * @{
 ******************************************************************************/

#define UTIL_SUPPLY_TYPE_UNKNOWN    0 /**< Unknown power supply type                */
#define UTIL_SUPPLY_TYPE_USB        1 /**< The board powered from the USB connector */
#define UTIL_SUPPLY_TYPE_AA         2 /**< The board powered from AA batteries      */
#define UTIL_SUPPLY_TYPE_AAA        3 /**< The board powered from AAA batteries     */
#define UTIL_SUPPLY_TYPE_CR2032     4 /**< The board powered from a CR2032 battery  */

uint32_t UTIL_init        (void);
void     UTIL_delay       (uint32_t ms);
void     UTIL_shutdown    (void);

uint32_t UTIL_sleepInit   (void);
void     UTIL_sleep       (uint32_t ms);
uint32_t UTIL_waitForEvent(uint32_t timeout);

void     UTIL_supplyProbe                (void);
void     UTIL_supplyGetCharacteristics   (uint8_t *type, float *voltage, float *ir);
uint8_t  UTIL_supplyGetType              (void);
bool     UTIL_isLowPower                 (void);

/** @} */
/** @} */

#endif // UTIL_H
