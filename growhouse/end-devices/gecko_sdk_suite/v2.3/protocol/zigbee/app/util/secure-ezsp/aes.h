/*
 * File: aes.h
 * Description: Interface definition for AES functionality.
 *
 * Copyright 2017 Silicon Laboratories, Inc.                                *80*
 */

#ifndef SILABS_APP_UTIL_SECURE_EZSP_AES_H
#define SILABS_APP_UTIL_SECURE_EZSP_AES_H

// This function loads the 16 byte key into the AES hardware accelerator.
void emLoadKeyIntoCore(const uint8_t* key);

// This function retrieves the 16 byte key from the AES hardware accelerator.
void emGetKeyFromCore(uint8_t* key);

// This function encrypts the 16 byte plaintext block with the previously-loaded
// 16 byte key using the AES hardware accelerator.
// The resulting 16 byte ciphertext is written to the block parameter,
// overwriting the plaintext.
void emStandAloneEncryptBlock(uint8_t* block);

// emAesEncrypt performs AES encryption in ECB mode on the plaintext pointed to
// by the block parameter, using the key pointed to by the key parameter, and
// places the resulting ciphertext into the 16 bytes of memory pointed to by the
// block parameter (overwriting the supplied plaintext).  Any existing key is
// destroyed.
void emAesEncrypt(uint8_t* block, const uint8_t* key);

// emAesDecrypt performs AES decryption in ECB mode on the ciphertext pointed to
// by the block parameter, using the key pointed to by the key parameter, and
// places the resulting plaintext into the 16 bytes of memory pointed to by the
// block parameter (overwriting the supplied cyphertext).  Any existing key is
// destroyed.
// This is only supported on EFR, because EM35x hardware doesn't support it
#if CORTEXM3_EFR32
void emAesDecrypt(uint8_t* block, const uint8_t* key);
#endif

#endif // SILABS_APP_UTIL_SECURE_EZSP_AES_H
