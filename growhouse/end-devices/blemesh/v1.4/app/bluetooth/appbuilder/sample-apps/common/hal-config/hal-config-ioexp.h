/***************************************************************************//**
 * @file
 * @brief hal-config-ioexp.h
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

#ifndef HAL_CONFIG_IOEXP_H
#define HAL_CONFIG_IOEXP_H

// $[IOEXP]
#define HAL_IOEXP_ENABLE                              (1)

#define BSP_IOEXP_PERIPHERAL                          (HAL_I2C_PORT_I2C0)
#define HAL_IOEXP_VCOM_ENABLE                         (1)
#define BSP_IOEXP_SDA_PIN                             (10)
#define BSP_IOEXP_SDA_PORT                            (gpioPortC)
#define BSP_IOEXP_SDA_LOC                             (15)

#define BSP_IOEXP_SCL_PIN                             (11)
#define BSP_IOEXP_SCL_PORT                            (gpioPortC)
#define BSP_IOEXP_SCL_LOC                             (15)

#define HAL_IOEXP_INIT_ENABLE                         (0)
// [IOEXP]$

#endif /* HAL_CONFIG_IOEXP_H */
