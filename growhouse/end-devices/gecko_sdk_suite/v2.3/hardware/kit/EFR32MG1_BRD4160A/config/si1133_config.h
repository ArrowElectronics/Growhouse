/***************************************************************************//**
 * @file si1133_config.h
 * @brief Si1133 Ambient Light and UV sensor configuration file
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

#ifndef SI1133_CONFIG_H
#define SI1133_CONFIG_H

#define SI1133_I2C_BUS_TIMEOUT          (1000)
#define SI1133_I2C_DEVICE               (I2C0)
#define SI1133_I2C_DEVICE_BUS_ADDRESS   (0xAA)

#endif // SI1133_CONFIG_H
