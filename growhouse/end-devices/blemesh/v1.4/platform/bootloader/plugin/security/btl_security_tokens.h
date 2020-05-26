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
#ifndef BTL_SECURITY_TOKENS_H
#define BTL_SECURITY_TOKENS_H

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup Tokens
 * @{
 * @brief Manufacturing token handling for the bootloader
 * @details
 ******************************************************************************/

/***************************************************************************//**
 * Get the X component of the ECDSA secp256r1 public key
 *
 * @return Pointer to X component of public key
 ******************************************************************************/
const uint8_t* btl_getSignedBootloaderKeyXPtr(void);

/***************************************************************************//**
 * Get the Y component of the ECDSA secp256r1 public key
 *
 * @return Pointer to Y component of public key
 ******************************************************************************/
const uint8_t* btl_getSignedBootloaderKeyYPtr(void);

/***************************************************************************//**
 * Get the AES-CCM encryption key
 *
 * @return Pointer to AES-CCM key
 ******************************************************************************/
const uint8_t* btl_getImageFileEncryptionKeyPtr(void);

/** @} addtogroup Tokens */
/** @} addtogroup Security */
/** @} addtogroup Plugin */

#endif // BTL_SECURITY_TOKENS_H
