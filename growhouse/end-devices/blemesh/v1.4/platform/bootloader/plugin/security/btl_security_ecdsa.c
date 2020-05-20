/***************************************************************************//**
 * @file
 * @brief ECDSA signing functionality for Silicon Labs bootloader
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
#include "btl_security_ecdsa.h"
#include "em_device.h"
#include "btl_security_tokens.h"

#include <stddef.h>
#include <string.h> // For memset

#if defined(CRYPTO_PRESENT)

#include "ecc/ecc.h"
#include "em_cmu.h"

#if !defined(CRYPTO) && defined(CRYPTO0)
#define cmuClock_CRYPTO cmuClock_CRYPTO0
#define CRYPTO CRYPTO0
#endif

/** Verify the ECDSA signature of the SHA hash, using
 *  the public key in the relevant token, with the signature contained in
 *  the byte arrays pointed to.
 */
int32_t btl_verifyEcdsaP256r1(const uint8_t *sha256,
                              const uint8_t *signatureR,
                              const uint8_t *signatureS,
                              const uint8_t *keyX,
                              const uint8_t *keyY)
{
  if ((sha256 == NULL) || (signatureR == NULL) || (signatureS == NULL)) {
    return BOOTLOADER_ERROR_SECURITY_INVALID_PARAM;
  }

  // Re-enable the clock of the CRYPTO module, since mbedtls turned it off
  CMU_ClockEnable(cmuClock_CRYPTO, true);

  ECC_Point_t pubkey;
  memset(&pubkey, 0, sizeof(ECC_Point_t));
  ECC_ByteArrayToBigInt(pubkey.X, keyX);
  ECC_ByteArrayToBigInt(pubkey.Y, keyY);

  ECC_EcdsaSignature_t ecc_signature;
  ECC_ByteArrayToBigInt(ecc_signature.r, signatureR);
  ECC_ByteArrayToBigInt(ecc_signature.s, signatureS);

  return ECC_ECDSA_VerifySignatureP256(CRYPTO,
                                       sha256,
                                       sizeof(ECC_BigInt_t),
                                       &pubkey,
                                       &ecc_signature);
}

#elif defined(SEMAILBOX_PRESENT)

#include "em_se.h"

/** Verify the ECDSA signature of the SHA hash, using
 *  the public key in the relevant token, with the signature contained in
 *  the byte arrays pointed to.
 */
int32_t btl_verifyEcdsaP256r1(const uint8_t *sha256,
                              const uint8_t *signatureR,
                              const uint8_t *signatureS,
                              const uint8_t *keyX,
                              const uint8_t *keyY)
{
  if ((sha256 == NULL) || (signatureR == NULL) || (signatureS == NULL)) {
    return BOOTLOADER_ERROR_SECURITY_INVALID_PARAM;
  }

  // Keyspec for secp256r1, given public signing key
  uint32_t keyspec = 0x80000000 | 31 | (1 << 10) | (1 << 13);

  SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_SIGNATURE_VERIFY);
  SE_addParameter(&command, keyspec); // 0 = key in host memory
  SE_addParameter(&command, 32); // length of hash

  if ((keyX == NULL) || (keyY == NULL)) {
    // TODO: NULL key means we should use the root key in the SE
  }

  SE_DataTransfer_t key_x = SE_DATATRANSFER_DEFAULT((uint8_t *)keyX, 32);
  SE_DataTransfer_t key_y = SE_DATATRANSFER_DEFAULT((uint8_t *)keyY, 32);
  SE_addDataInput(&command, &key_x);
  SE_addDataInput(&command, &key_y);

  SE_DataTransfer_t hash = SE_DATATRANSFER_DEFAULT((uint8_t *)sha256, 32);
  SE_addDataInput(&command, &hash);

  SE_DataTransfer_t signature_r = SE_DATATRANSFER_DEFAULT((uint8_t *)signatureR, 32);
  SE_DataTransfer_t signature_s = SE_DATATRANSFER_DEFAULT((uint8_t *)signatureS, 32);
  SE_addDataInput(&command, &signature_r);
  SE_addDataInput(&command, &signature_s);

  SE_executeCommand(&command);

  SE_Response_t rsp = SE_readCommandResponse();

  if (rsp == SE_RESPONSE_OK) {
    return BOOTLOADER_OK;
  }

  return BOOTLOADER_ERROR_SECURITY_REJECTED;
}
#endif
