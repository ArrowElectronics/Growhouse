/***************************************************************************//**
 * @file btl_security_tokens.c
 * @brief Manufacturing token handling
 * @version 1.6.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "btl_security_tokens.h"
#include "em_device.h"

const uint8_t* btl_getSignedBootloaderKeyXPtr(void)
{
  return (const uint8_t*)(LOCKBITS_BASE + 0x34A);
}

const uint8_t* btl_getSignedBootloaderKeyYPtr(void)
{
  return (const uint8_t*)(LOCKBITS_BASE + 0x36A);
}

const uint8_t* btl_getImageFileEncryptionKeyPtr(void)
{
  return (const uint8_t*)(LOCKBITS_BASE + 0x286);
}
