/***************************************************************************//**
 * @file btl_config.h
 * @brief Configuration for bootloader
 * @author Silicon Labs
 * @version 1.6.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef BTL_CONFIG_H
#define BTL_CONFIG_H

//
// Bootloader Version
//
#define BOOTLOADER_VERSION_MAIN_MAJOR             1
#define BOOTLOADER_VERSION_MAIN_MINOR             6
#ifndef BOOTLOADER_VERSION_MAIN_CUSTOMER
#define BOOTLOADER_VERSION_MAIN_CUSTOMER          0
#endif

#define BOOTLOADER_VERSION_MAIN (BOOTLOADER_VERSION_MAIN_MAJOR   << 24 \
                                 | BOOTLOADER_VERSION_MAIN_MINOR << 16 \
                                 | BOOTLOADER_VERSION_MAIN_CUSTOMER)

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

#endif
