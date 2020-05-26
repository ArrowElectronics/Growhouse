/***************************************************************************//**
 * @file
 * @brief Bootloader upgrade functionality for the Silicon Labs bootloader
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
#ifndef BTL_UPGRADE_H
#define BTL_UPGRADE_H

#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup Core
 * @{
 * @addtogroup Upgrade
 * @brief Methods to verify and upgrade the main bootloader
 * @details
 * @{
 ******************************************************************************/

/**
 * Check if there is a bootloader upgrade available.
 *
 * @return True if there is an upgrade image in the upgrade location
 */
bool btl_checkForUpgrade(void);

/**
 * Apply a bootloader upgrade.
 *
 * @return True if the bootloader upgrade was applied successfully.
 */
bool btl_applyUpgrade(void);

/** @} addtogroup upgrade */
/** @} addtogroup core */

#endif // BTL_UPGRADE_H
