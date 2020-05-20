/***************************************************************************//**
 * @file
 * @brief Manufacturing token handling
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
#include "btl_security_tokens.h"
#include "em_device.h"

#if defined(LOCKBITS_BASE)
#define PUBKEY_OFFSET_X (0x34A)
#define PUBKEY_OFFSET_Y (0x36A)
#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_200)
// Lockbits are placed in the topmost flash page
#define LOCKBITS_BASE ((FLASH_BASE) + (FLASH_SIZE) -(FLASH_PAGE_SIZE))
#define PUBKEY_OFFSET_X (0x34C)
#define PUBKEY_OFFSET_Y (0x36C)
#else
#error "Lockbits location unknown"
#endif

const uint8_t* btl_getSignedBootloaderKeyXPtr(void)
{
  return (const uint8_t*)(LOCKBITS_BASE + PUBKEY_OFFSET_X);
}

const uint8_t* btl_getSignedBootloaderKeyYPtr(void)
{
  return (const uint8_t*)(LOCKBITS_BASE + PUBKEY_OFFSET_Y);
}

const uint8_t* btl_getImageFileEncryptionKeyPtr(void)
{
  return (const uint8_t*)(LOCKBITS_BASE + 0x286);
}
