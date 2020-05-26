/***************************************************************************//**
 * @file si7021.h
 * @brief Driver for the Si7021 I2C Humidity and Temperature Sensor
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

#ifndef SI7021_H
#define SI7021_H

#include <stdlib.h>
#include <stdint.h>
#include "si7021_config.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup Si7021
 * @{
 ******************************************************************************/

/**************************************************************************//**
* @name Error Codes
* @{
******************************************************************************/
#define SI7021_OK                             0       /**< No errors                   */
#define SI7021_ERROR_I2C_TRANSFER_NACK        0x0001  /**< No acknowledgement received */
#define SI7021_ERROR_I2C_TRANSFER_FAILED      0x0002  /**< I2C transaction failed      */
#define SI7021_ERROR_INVALID_DEVICE_ID        0x0003  /**< The device ID is invalid    */
#define SI7021_ERROR_TIMEOUT                  0x0004  /**< Timeout occurred            */

/**************************************************************************//**
* @name Si7021 Commands
* @{
******************************************************************************/
#define SI7021_CMD_MEASURE_RH            0xE5            /**< Measure Relative Humidity, Hold Master Mode */
#define SI7021_CMD_MEASURE_RH_NO_HOLD    0xF5            /**< Measure Relative Humidity, No Hold Master Mode */
#define SI7021_CMD_MEASURE_TEMP          0xE3            /**< Measure Temperature, Hold Master Mode */
#define SI7021_CMD_MEASURE_TEMP_NO_HOLD  0xF3            /**< Measure Temperature, No Hold Master Mode */
#define SI7021_CMD_READ_TEMP             0xE0            /**< Read Temperature Value from Previous RH Measurement */
#define SI7021_CMD_RESET                 0xFE            /**< Reset */
#define SI7021_CMD_WRITE_USER_REG1       0xE6            /**< Write RH/T User Register 1 */
#define SI7021_CMD_READ_USER_REG1        0xE7            /**< Read RH/T User Register 1 */
#define SI7021_CMD_WRITE_HEATER_CTRL     0x51            /**< Write Heater Control Register */
#define SI7021_CMD_READ_HEATER_CTRL      0x11            /**< Read Heater Control Register */
#define SI7021_CMD_READ_ID_BYTE1         { 0xFA, 0x0F }  /**< Read Electronic ID 1st Byte */
#define SI7021_CMD_READ_ID_BYTE2         { 0xFC, 0xC9 }  /**< Read Electronic ID 2nd Byte */
#define SI7021_CMD_READ_FW_REV           { 0x84, 0xB8 }  /**< Read Firmware Revision */
/**@}*/

uint32_t SI7021_init     (void);
void     SI7021_deInit   (void);
uint32_t SI7021_measure  (uint32_t *rhData, int32_t *tData);
uint32_t SI7021_getFwRev (uint8_t *fwRev);

uint32_t SI7021_cmdWrite (uint8_t *cmd, size_t cmdLen, uint8_t *data, size_t dataLen);
uint32_t SI7021_cmdRead  (uint8_t *cmd, size_t cmdLen, uint8_t *result, size_t resultLen);

/** @} */
/** @} */

#endif // SI7021_H
