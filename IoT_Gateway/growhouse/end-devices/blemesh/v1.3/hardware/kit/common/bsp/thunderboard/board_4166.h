/***************************************************************************//**
 * @file board_4166.h
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

#ifndef BOARD_4166_H
#define BOARD_4166_H

#include <stdint.h>

typedef void (*BOARD_IrqCallback)(void);/**< Interrupt callback function type definition */

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup BOARD_4166
 * @{
 ******************************************************************************/

void     BOARD_gasSensorSetIRQCallback(BOARD_IrqCallback cb);

uint32_t BOARD_hallSensorEnable    (bool enable);
uint32_t BOARD_hallSensorEnableIRQ (bool enable);
void     BOARD_hallSensorSetIRQCallback(BOARD_IrqCallback cb);
void     BOARD_hallSensorClearIRQ(void);

void     BOARD_alsSetIRQCallback(BOARD_IrqCallback cb);

void     BOARD_imuSetIRQCallback(BOARD_IrqCallback cb);

void     BOARD_pushButton0SetIRQCallback(BOARD_IrqCallback cb);
void     BOARD_pushButton0ClearIRQ(void);
void     BOARD_pushButton1SetIRQCallback(BOARD_IrqCallback cb);
void     BOARD_pushButton1ClearIRQ(void);

/** @} */
/** @} */

#endif // BOARD_4166_H
