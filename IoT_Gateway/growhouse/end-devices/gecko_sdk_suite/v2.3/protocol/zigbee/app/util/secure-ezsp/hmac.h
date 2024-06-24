/*
 * File: hmac.h
 * Description: HMAC (keyed-hash message authentication code) API.
 *
 * Copyright 2017 Silicon Laboratories, Inc.                                *80*
 */

#ifndef SILABS_APP_UTIL_SECURE_EZSP_HMAC_H
#define SILABS_APP_UTIL_SECURE_EZSP_HMAC_H

/*
 * Keyed hash function for message authentication
 *
 * This is HMAC (see FIPS PUB 198) using the AES hash.  HMAC is a
 * method for computing a hash from a key and a data message using
 * a message-only hash function.
 */
void emberHmacAesHash(const uint8_t *key,
                      const uint8_t *data,
                      uint8_t dataLength,
                      uint8_t *result);

#endif // SILABS_APP_UTIL_SECURE_EZSP_HMAC_H
