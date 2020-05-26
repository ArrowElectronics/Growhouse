/***************************************************************************//**
 * @file
 * @brief NVM3 driver HAL for memory mapped RAM
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef NVM3_HAL_RAM_H
#define NVM3_HAL_RAM_H

#include "nvm3_hal.h"
#include "nvm3_hal_host.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup emdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup NVM3
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup NVM3HalRam
 * @brief NVM3 HAL RAM module
 * @{
 * @details
 * This module provides the RAM interface to the NVM.
 *
 * @note The features available through the handle are used by the NVM3 and
 * should not be used directly by any applications.
 ******************************************************************************/

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   ******************************
 ******************************************************************************/

extern const nvm3_HalHandle_t nvm3_halRamHandle;
extern const nvm3_HalConfig_t nvm3_halRamConfig;

/** @} (end addtogroup NVM3Hal) */
/** @} (end addtogroup NVM3) */
/** @} (end addtogroup emdrv) */

#ifdef __cplusplus
}
#endif

#endif /* NVM3_HAL_RAM_H */
