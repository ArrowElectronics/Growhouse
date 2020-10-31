/***************************************************************************//**
 * @file HALL_config.h
 * @brief Si7210 Hall Effect Sensor configuration file
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
#ifndef HALL_CONFIG_H
#define HALL_CONFIG_H

#define HALL_GPIO_PORT_OUT   gpioPortB
#define HALL_GPIO_PIN_OUT    11

#define SI7210_I2C_BUS_TIMEOUT  1000
#define SI7210_I2C_BUS_ADDRESS  0x30
#define SI7210_DEVICE_ID        0x13
#define SI7210_CHIP_ID          0x01
#define SI7210_REV_ID_MIN       0x03
#define SI7210_REV_ID_MAX       0x04

#define SI7210_I2C_DEVICE       (I2C1)
#define SI7210_SDA_LOCATION     (I2C_ROUTELOC0_SDALOC_LOC8)
#define SI7210_SCL_LOCATION     (I2C_ROUTELOC0_SCLLOC_LOC8)
#define SI7210_SDA_LOC          8
#define SI7210_SCL_LOC          8
#define SI7210_SDA_PORT         gpioPortB
#define SI7210_SDA_PIN          8
#define SI7210_SCL_PORT         gpioPortB
#define SI7210_SCL_PIN          9

#define I2CSPM_INIT_SI7210                                                   \
  { SI7210_I2C_DEVICE,        /* I2C instance                             */ \
    SI7210_SCL_PORT,          /* SCL port                                 */ \
    SI7210_SCL_PIN,           /* SCL pin                                  */ \
    SI7210_SDA_PORT,          /* SDA port                                 */ \
    SI7210_SDA_PIN,           /* SDA pin                                  */ \
    SI7210_SCL_LOC,           /* Port location of SCL signal              */ \
    SI7210_SDA_LOC,           /* Port location of SDA signal              */ \
    0,                        /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,    /* Set to standard rate                     */ \
    i2cClockHLRStandard,      /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // HALL_CONFIG_H
