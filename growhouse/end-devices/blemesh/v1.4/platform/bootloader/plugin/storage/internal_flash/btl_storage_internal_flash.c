/***************************************************************************//**
 * @file
 * @brief Internal flash storage plugin for Silicon Labs Bootloader.
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

#include "config/btl_config.h"

#include "core/flash/btl_internal_flash.h"

#include "plugin/storage/btl_storage.h"
#include "plugin/storage/internal_flash/btl_storage_internal_flash.h"

#include "plugin/debug/btl_debug.h"

#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_device.h"
MISRAC_ENABLE

#include <string.h>

// -----------------------------------------------------------------------------
// Globals

const BootloaderStorageLayout_t storageLayout = {
  INTERNAL_FLASH,
  BTL_PLUGIN_STORAGE_NUM_SLOTS,
  BTL_PLUGIN_STORAGE_SLOTS
};

// -----------------------------------------------------------------------------
// Statics

#if defined(_SILICON_LABS_32B_SERIES_2)
#define FLASH_ALIGNMENT   (4)
#else
#define FLASH_ALIGNMENT   (2)
#endif

static const BootloaderStorageImplementationInformation_t deviceInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  40, // Datasheet EFR32MG1: max 40 ms for page erase
  40, // Datasheet EFR32MG1: max 40 ms for mass erase
  FLASH_PAGE_SIZE,
  FLASH_SIZE,
  "EFR32",
  FLASH_ALIGNMENT
};

// -----------------------------------------------------------------------------
// Functions

// --------------------------------
// Internal Functions

const BootloaderStorageImplementationInformation_t * getDeviceInfo(void)
{
  return &deviceInfo;
}

static bool verifyAddressRange(uint32_t address,
                               uint32_t length)
{
  // Flash starts at 0, and is FLASH_SIZE large
  if ((address + length) <= FLASH_SIZE) {
    return true;
  } else {
    return false;
  }
}

static bool verifyErased(uint32_t address,
                         uint32_t length)
{
  for (uint32_t i = 0; i < length; i += 4) {
    if (*(uint32_t *)(address + i) != 0xFFFFFFFF) {
      return false;
    }
  }
  return true;
}

// --------------------------------
// API Functions

int32_t storage_init(void)
{
  return BOOTLOADER_OK;
}

bool storage_isBusy(void)
{
  return MSC->STATUS & MSC_STATUS_BUSY;
}

int32_t storage_readRaw(uint32_t address, uint8_t *data, size_t length)
{
  // Ensure address is is within flash
  if (!verifyAddressRange(address, length)) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }

  memcpy(data, (void *)address, length);

  return BOOTLOADER_OK;
}

int32_t storage_writeRaw(uint32_t address, uint8_t *data, size_t numBytes)
{
  // Ensure address is is within chip
  if (!verifyAddressRange(address, numBytes)) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }
  // Ensure space is empty
  if (!verifyErased(address, numBytes)) {
    return BOOTLOADER_ERROR_STORAGE_NEEDS_ERASE;
  }

  if (flash_writeBuffer(address, data, numBytes)) {
    return BOOTLOADER_OK;
  } else {
    // TODO: Better return code
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }
}

int32_t storage_eraseRaw(uint32_t address, size_t totalLength)
{
  // Ensure erase covers an integer number of pages
  if (totalLength % FLASH_PAGE_SIZE) {
    return BOOTLOADER_ERROR_STORAGE_NEEDS_ALIGN;
  }
  // Ensure erase is page aligned
  if (address % FLASH_PAGE_SIZE) {
    return BOOTLOADER_ERROR_STORAGE_NEEDS_ALIGN;
  }
  // Ensure address is is within flash
  if (!verifyAddressRange(address, totalLength)) {
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }

  bool retval = false;

  do {
    retval = flash_erasePage(address);
    address += FLASH_PAGE_SIZE;
    totalLength -= FLASH_PAGE_SIZE;
  } while (totalLength > 0 && retval);

  if (retval) {
    return BOOTLOADER_OK;
  } else {
    // TODO: Better return code?
    return BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS;
  }
}

int32_t storage_shutdown(void)
{
  return BOOTLOADER_OK;
}
