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
#define SI1133_I2C_DEVICE_BUS_ADDRESS   (0xAA)

#define SI1133_I2C_DEVICE      (I2C1)
#define SI1133_SDA_LOCATION    (I2C_ROUTELOC0_SDALOC_LOC17)
#define SI1133_SCL_LOCATION    (I2C_ROUTELOC0_SCLLOC_LOC17)
#define SI1133_SDA_LOC         17
#define SI1133_SCL_LOC         17
#define SI1133_SDA_PORT        gpioPortC
#define SI1133_SDA_PIN         4
#define SI1133_SCL_PORT        gpioPortC
#define SI1133_SCL_PIN         5

#define I2CSPM_INIT_SI1133                                                \
  { SI1133_I2C_DEVICE,     /* I2C instance                             */ \
    SI1133_SCL_PORT,       /* SCL port                                 */ \
    SI1133_SCL_PIN,        /* SCL pin                                  */ \
    SI1133_SDA_PORT,       /* SDA port                                 */ \
    SI1133_SDA_PIN,        /* SDA pin                                  */ \
    SI1133_SCL_LOC,        /* Port location of SCL signal              */ \
    SI1133_SDA_LOC,        /* Port location of SDA signal              */ \
    0,                     /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX, /* Set to standard rate                     */ \
    i2cClockHLRStandard,   /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // SI1133_CONFIG_H
