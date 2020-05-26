/***************************************************************************//**
 * @file
 * @brief Configuration for bootloader
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
#ifndef BTL_CONFIG_H
#define BTL_CONFIG_H

//
// Bootloader Version
//
#define BOOTLOADER_VERSION_MAIN_MAJOR             1
#define BOOTLOADER_VERSION_MAIN_MINOR             8
#ifndef BOOTLOADER_VERSION_MAIN_CUSTOMER
#define BOOTLOADER_VERSION_MAIN_CUSTOMER          2
#endif

#define BOOTLOADER_VERSION_MAIN (BOOTLOADER_VERSION_MAIN_MAJOR   << 24 \
                                 | BOOTLOADER_VERSION_MAIN_MINOR << 16 \
                                 | BOOTLOADER_VERSION_MAIN_CUSTOMER)

#include "core/btl_util.h"

MISRAC_DISABLE
#include "em_device.h"
MISRAC_ENABLE

//
// Bootloader configuration
//
#include BTL_CONFIG_FILE

#ifdef BTL_SLOT_CONFIGURATION
  #include BTL_SLOT_CONFIGURATION
#endif

//
// HAL Configuration
//
#ifndef LIBRARY_BUILD
#include "hal-config.h"
#endif

//
// Option validation
//

#if defined(BTL_UPGRADE_LOCATION_BASE)
// The upgrade location needs to fit upgrades of up to 42k
#if ((BTL_UPGRADE_LOCATION_BASE + 0x0000C000UL) > FLASH_SIZE) \
  || (BTL_UPGRADE_LOCATION_BASE % FLASH_PAGE_SIZE)
#error "Invalid bootloader upgrade base address"
#endif
#endif

#endif
