/***************************************************************************//**
 * @file si7021_config.h
 * @brief Si7021 I2C Humidity and Temperature Sensor configuration file
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

#ifndef SI7021_CONFIG_H
#define SI7021_CONFIG_H

#define SI7021_I2C_BUS_ADDRESS     (0x40)
#define SI7021_DEVICE_ID           (0x15)

#define SI7021_I2C_DEVICE      (I2C1)
#define SI7021_SDA_LOCATION    (I2C_ROUTELOC0_SDALOC_LOC17)
#define SI7021_SCL_LOCATION    (I2C_ROUTELOC0_SCLLOC_LOC17)
#define SI7021_SDA_LOC         17
#define SI7021_SCL_LOC         17
#define SI7021_SDA_PORT        gpioPortC
#define SI7021_SDA_PIN         4
#define SI7021_SCL_PORT        gpioPortC
#define SI7021_SCL_PIN         5

#define I2CSPM_INIT_SI7021                                                   \
  { SI7021_I2C_DEVICE,        /* I2C instance                             */ \
    SI7021_SCL_PORT,          /* SCL port                                 */ \
    SI7021_SCL_PIN,           /* SCL pin                                  */ \
    SI7021_SDA_PORT,          /* SDA port                                 */ \
    SI7021_SDA_PIN,           /* SDA pin                                  */ \
    SI7021_SCL_LOC,           /* Port location of SCL signal              */ \
    SI7021_SDA_LOC,           /* Port location of SDA signal              */ \
    0,                        /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,    /* Set to standard rate                     */ \
    i2cClockHLRStandard,      /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // SI7021_CONFIG_H
