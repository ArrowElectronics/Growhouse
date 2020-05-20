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

#define BAP_I2C_BUS_TIMEOUT         (1000)
#define BAP_I2C_DEVICE              (I2C1)
#define BAP_I2C_BUS_ADDRESS         (0x77)

#define BAP_SDA_LOCATION     (I2C_ROUTELOC0_SDALOC_LOC17)
#define BAP_SCL_LOCATION     (I2C_ROUTELOC0_SCLLOC_LOC17)
#define BAP_SDA_LOC          17
#define BAP_SCL_LOC          17
#define BAP_SDA_PORT         gpioPortC
#define BAP_SDA_PIN          4
#define BAP_SCL_PORT         gpioPortC
#define BAP_SCL_PIN          5

#define I2CSPM_INIT_BAP                                                      \
  { BAP_DEVICE,               /* I2C instance                             */ \
    BAP_SCL_PORT,             /* SCL port                                 */ \
    BAP_SCL_PIN,              /* SCL pin                                  */ \
    BAP_SDA_PORT,             /* SDA port                                 */ \
    BAP_SDA_PIN,              /* SDA pin                                  */ \
    BAP_SCL_LOC,              /* Port location of SCL signal              */ \
    BAP_SDA_LOC,              /* Port location of SDA signal              */ \
    0,                        /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,    /* Set to standard rate                     */ \
    i2cClockHLRStandard,      /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // BAP_CONFIG_H
