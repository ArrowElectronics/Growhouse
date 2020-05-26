/***************************************************************************//**
 * @file
 * @brief Bosch Sensortec BMP280 barometric pressure sensor configuration file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef BAP_CONFIG_H
#define BAP_CONFIG_H

#define BAP_I2C_BUS_TIMEOUT  (1000)
#define BAP_I2C_DEVICE       (I2C0)
#define BAP_I2C_BUS_ADDRESS  (0x77)

#endif // BAP_CONFIG_H
