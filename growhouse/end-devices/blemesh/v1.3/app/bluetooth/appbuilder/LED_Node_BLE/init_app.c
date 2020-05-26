/***************************************************************************//**
 * @file init_app.c
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#include "hal-config.h"
#else
#include "bspconfig.h"
#endif

#include "bsp.h"
#include "i2cspm.h"
#include "pti.h"

void initApp(void)
{
  // Enable PTI
  configEnablePti();

#if defined(HAL_VCOM_ENABLE)
  // Enable VCOM if requested
  GPIO_PinModeSet(BSP_VCOM_ENABLE_PORT, BSP_VCOM_ENABLE_PIN, gpioModePushPull, HAL_VCOM_ENABLE);
#endif // HAL_VCOM_ENABLE

#if (HAL_I2CSENSOR_ENABLE)
  // Initialize I2C peripheral
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  I2CSPM_Init(&i2cInit);
#endif // HAL_I2CSENSOR_ENABLE

#if defined(HAL_I2CSENSOR_ENABLE) || defined(HAL_SPIDISPLAY_ENABLE)
#if HAL_I2CSENSOR_ENABLE || HAL_SPIDISPLAY_ENABLE
#define DISPLAY_SENSOR_COMMON_ENABLE 1
#else
#define DISPLAY_SENSOR_COMMON_ENABLE 0
#endif
  //Enable I2C sensor if requested
  GPIO_PinModeSet(BSP_I2CSENSOR_ENABLE_PORT, BSP_I2CSENSOR_ENABLE_PIN, gpioModePushPull, DISPLAY_SENSOR_COMMON_ENABLE);
#endif
}