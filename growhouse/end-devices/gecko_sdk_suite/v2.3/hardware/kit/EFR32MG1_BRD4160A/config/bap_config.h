/***************************************************************************//**
 * @file bap_config.h
 * @brief Bosch Sensortec BMP280 barometric pressure sensor configuration file
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

#ifndef BAP_CONFIG_H
#define BAP_CONFIG_H

#define BAP_I2C_BUS_TIMEOUT  (1000)
#define BAP_I2C_DEVICE       (I2C0)
#define BAP_I2C_BUS_ADDRESS  (0x77)

#endif // BAP_CONFIG_H
