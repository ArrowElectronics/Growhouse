/***************************************************************************//**
 * @file
 * @brief Configuration for enabling CRYPTO hardware acceleration required for
 *  Bluetooth stack.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H
#include "em_device.h"
#include <stddef.h>
#include <stdint.h>
// bg_calloc and bg_free are implemented in bgcommon
void bg_free(void *pv);
void *bg_calloc(size_t num, size_t size);

#if !defined(NO_CRYPTO_ACCELERATION)
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_ECP_INTERNAL_ALT
#define ECP_SHORTWEIERSTRASS
#define MBEDTLS_AES_ALT
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_ECP_ADD_MIXED_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_ECP_RANDOMIZE_JAC_ALT
#define CRYPTO_DEVICE_PREEMPTION
#endif /* CRYPTO_COUNT */

// use a different method of abstraction for devices with SE
#if defined(SEMAILBOX_PRESENT)
#include "em_se.h"
#if defined(SE_COMMAND_CREATE_KEY)
#define MBEDTLS_ECDH_GEN_PUBLIC_ALT
#endif
#if defined(SE_COMMAND_DH)
#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
#endif
#if defined(SE_COMMAND_AES_CMAC)
#define MBEDTLS_CMAC_ALT
#endif
#if defined(SE_COMMAND_OPTION_HASH_SHA256) || defined(SE_COMMAND_OPTION_HASH_SHA224)
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA256_PROCESS_ALT
#endif
#if defined(SE_COMMAND_AES_CCM_ENCRYPT) && defined(SE_COMMAND_AES_CCM_DECRYPT)
#define MBEDTLS_CCM_ALT
#endif
#define MBEDTLS_AES_ALT
#endif /* SEMAILBOX_PRESENT */

#endif /* NO_CRYPTO_ACCELERATION */

/* mbed TLS modules */
#define MBEDTLS_CIPHER_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECDH_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CMAC_C
#define MBEDTLS_CCM_C
#define MBEDTLS_AES_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_CIPHER_MODE_CTR
#undef MBEDTLS_FS_IO
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_CALLOC_MACRO  bg_calloc /**< Default allocator macro to use, can be undefined */
#define MBEDTLS_PLATFORM_FREE_MACRO    bg_free /**< Default free macro to use, can be undefined */

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS   256
#define MBEDTLS_MPI_MAX_SIZE    32 // 384 bits is 48 bytes

/*
   Set MBEDTLS_ECP_WINDOW_SIZE to configure
   ECC point multiplication window size, see ecp.h:
   2 = Save RAM at the expense of speed
   3 = Improve speed at the expense of RAM
   4 = Optimize speed at the expense of RAM
 */
#define MBEDTLS_ECP_WINDOW_SIZE        2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

/* Significant speed benefit at the expense of some ROM */
/* Do not define if only enabling curves that have HW support */
//#define MBEDTLS_ECP_NIST_OPTIM

/* Do not compile in SW ECP code if only enabling curves that have hardware support */
#define MBEDTLS_ECP_NO_FALLBACK

/*
 * You should adjust this to the exact number of sources you're using: default
 * is the "mbedtls_platform_entropy_poll" source, but you may want to add other ones.
 * Minimum is 2 for the entropy test suite.
 */
#define MBEDTLS_ENTROPY_MAX_SOURCES 2

#include "check_config.h"

#endif /* MBEDTLS_CONFIG_H */
