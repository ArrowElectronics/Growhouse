/***************************************************************************//**
 * @file ccs811_config.h
 * @brief Cambridge CMOS Sensors CCS811 gas sensor configuration file
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

#ifndef CCS811_CONFIG_H
#define CCS811_CONFIG_H

#define CCS811_I2C_BUS_TIMEOUT          1000  /**< I2C bus timeout                            */
#define CCS811_I2C_DEVICE               I2C0  /**< I2C device used to control the CCS811      */
#define CCS811_BUS_ADDRESS              0xB4  /**< I2C bus address of the CCS881              */
#define CCS811_HW_ID                    0x81  /**< The hardware ID of the CCS881              */
#define CCS811_FIRMWARE_UPDATE          0     /**< Enable or disable CCS811 firmware upgrade  */

#endif // CCS811_CONFIG_H
