/***************************************************************************//**
 * @file
 * @brief Storage plugin for the Gecko Bootloader.
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

#include "api/btl_interface.h"
#include "btl_storage.h"

#include "core/btl_reset.h"
#include "core/btl_parse.h"
#include "core/btl_bootload.h"
#include "plugin/debug/btl_debug.h"

// --------------------------------
// Prototypes

static int32_t installImageFromSlot(int32_t slotId);

// --------------------------------
// Function implementations

uint32_t storage_getBaseAddress(void)
{
#ifdef BTL_STORAGE_BASE_ADDRESS
  return BTL_STORAGE_BASE_ADDRESS;
#else
  return 0;
#endif
}

int32_t storage_main(void)
{
#if BTL_STORAGE_BOOTLOAD_LIST_LENGTH == 1
  return installImageFromSlot(0);
#else
  int32_t ret;

  int32_t slotIds[BTL_STORAGE_BOOTLOAD_LIST_LENGTH] = { -1 };

  ret = storage_getBootloadList(slotIds, BTL_STORAGE_BOOTLOAD_LIST_LENGTH);
  if (ret != BOOTLOADER_OK) {
    BTL_DEBUG_PRINTLN("BI err");
    return ret;
  }

  // Attempt to bootload given images in sequence
  for (size_t id = 0; id < BTL_STORAGE_BOOTLOAD_LIST_LENGTH; id++) {
    if (slotIds[id] == -1) {
      // Invalid slot ID; try the next one
      continue;
    }

    ret = installImageFromSlot(slotIds[id]);
    if (ret == BOOTLOADER_OK) {
      break;
    }
  }

  return ret;
#endif
}

static int32_t installImageFromSlot(int32_t slotId)
{
  BootloaderParserContext_t parseContext;
  int32_t ret;

  BTL_DEBUG_PRINT("Slot: ");
  BTL_DEBUG_PRINT_WORD_HEX(slotId);
  BTL_DEBUG_PRINT_LF();

  // Get info about the image marked for bootloading
  storage_initParseSlot(slotId,
                        &parseContext,
                        sizeof(BootloaderParserContext_t));

  ret = BOOTLOADER_ERROR_PARSE_CONTINUE;
  do {
    ret = storage_verifySlot(&parseContext, NULL);
  } while (ret == BOOTLOADER_ERROR_PARSE_CONTINUE);

  if (ret != BOOTLOADER_ERROR_PARSE_SUCCESS) {
    // Image in slot is non-existant or corrupt.
    // Continue to next image
    BTL_DEBUG_PRINTLN("Verify fail");
    return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
  }

  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER)
      && !(parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_APPLICATION)) {
    BTL_DEBUG_PRINTLN("BL upg with no app");
    return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
  }

#if defined(SEMAILBOX_PRESENT)
  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_SE)
      && bootload_checkSeUpgradeVersion(parseContext.imageProperties.seUpgradeVersion)) {
    // SE upgrade should be applied
    if (storage_upgradeSeFromSlot(parseContext.slotId)) {
      if (!bootload_commitSeUpgrade(parser_getBootloaderUpgradeAddress())) {
        BTL_DEBUG_PRINTLN("SE upgrade commit fail");
      }
    } else {
      BTL_DEBUG_PRINTLN("SE upgrade parse fail");
    }
  }
#endif

  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER)) {
    BTL_DEBUG_PRINT("BL upg ");
    BTL_DEBUG_PRINT_WORD_HEX(mainBootloaderTable->header.version);
    BTL_DEBUG_PRINT(" -> ");
    BTL_DEBUG_PRINT_WORD_HEX(parseContext.imageProperties.bootloaderVersion);
    BTL_DEBUG_PRINT_LF();
  }

  if ((parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_BOOTLOADER)
      && (parseContext.imageProperties.bootloaderVersion
          > mainBootloaderTable->header.version)) {
    // This is a bootloader upgrade, and we also have an application
    // available for after the bootloader upgrade is complete
    if (storage_bootloadBootloaderFromSlot(
          parseContext.slotId,
          parseContext.imageProperties.bootloaderVersion)) {
      if (!bootload_commitBootloaderUpgrade(parser_getBootloaderUpgradeAddress(), parseContext.imageProperties.bootloaderUpgradeSize)) {
        // Bootloader upgrade failed; not a valid image
        BTL_DEBUG_PRINTLN("Btl upgrade commit fail");
      }
    } else {
      // Bootloader upgrade failed; not a valid image
      BTL_DEBUG_PRINTLN("Btl upgrade fail");
    }
    return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
  } else {
    // This should be an application upgrade
    if (!(parseContext.imageProperties.contents & BTL_IMAGE_CONTENT_APPLICATION)) {
      // ...but there is no app in the EBL
      BTL_DEBUG_PRINTLN("No app in slot");
      // Continue to next image
      return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
    } else if (!storage_bootloadApplicationFromSlot(
                 parseContext.slotId,
                 parseContext.imageProperties.application.version)) {
      // App upgrade failed.
      BTL_DEBUG_PRINTLN("App upgrade fail");
      // Continue to next image
      return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
    } else {
      // Application was updated. Reboot into new image.
    }
  }

  return BOOTLOADER_OK;
}
