/***************************************************************************//**
 * @file board_4160.h
 * @brief BOARD module header file
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

#ifndef BOARD_4160_H
#define BOARD_4160_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*BOARD_IrqCallback)(void);  /**< Interrupt callback function type definition */

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup BOARD_4160
 * @{
 ******************************************************************************/

uint32_t BOARD_picIntGet           (uint8_t *flags);
uint32_t BOARD_picIntClear         (uint8_t flags);
uint32_t BOARD_picWriteReg         (uint8_t addr, uint8_t value);
uint32_t BOARD_picReadReg          (uint8_t addr, uint8_t *result);
uint32_t BOARD_picRegBitsSet       (uint8_t addr, bool set, uint8_t bitMask);
uint32_t BOARD_picGetDeviceId      (void);
uint32_t BOARD_picGetFwRevision    (uint8_t *major, uint8_t *minor, uint8_t *patch);
uint8_t  BOARD_picGetHwRevision    (void);
bool     BOARD_picIsLegacyIntCtrl  (void);

void     BOARD_gasSensorSetIRQCallback   (BOARD_IrqCallback cb);
void     BOARD_alsSetIRQCallback         (BOARD_IrqCallback cb);
void     BOARD_imuSetIRQCallback         (BOARD_IrqCallback cb);
void     BOARD_pushButton0SetIRQCallback (BOARD_IrqCallback cb);
void     BOARD_pushButton0ClearIRQ       (void);
void     BOARD_pushButton1SetIRQCallback (BOARD_IrqCallback cb);
void     BOARD_pushButton1ClearIRQ       (void);

/** @} */
/** @} */

#endif // BOARD_4160_H
