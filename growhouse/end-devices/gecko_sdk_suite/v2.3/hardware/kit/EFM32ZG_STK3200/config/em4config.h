/***************************************************************************//**
 * @file
 * @brief Provide configuration parameters for EM4 wakeup button.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __SILICON_LABS_EM4CONFIG_H
#define __SILICON_LABS_EM4CONFIG_H

#include "bspconfig.h"

#define EM4_WU_PB           PB1
#define EM4_WU_PB_EN        GPIO_EM4WUEN_EM4WUEN_C9
#define EM4_WU_PB_PIN       BSP_GPIO_PB1_PIN
#define EM4_WU_PB_PORT      BSP_GPIO_PB1_PORT
#define EM4_WU_PB_STR       "PB1"

#define EM4_NON_WU_PB          PB0
#define EM4_NON_WU_PB_PIN      BSP_GPIO_PB0_PIN
#define EM4_NON_WU_PB_PORT     BSP_GPIO_PB0_PORT
#define EM4_NON_WU_PB_STR      "PB0"

#endif /* __SILICON_LABS_EM4CONFIG_H */
