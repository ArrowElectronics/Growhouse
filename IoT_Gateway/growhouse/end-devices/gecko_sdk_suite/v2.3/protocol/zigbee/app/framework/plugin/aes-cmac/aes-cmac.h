// Copyright 2014 Silicon Laboratories, Inc.
//
// Implementation of the Cipher-based Message Authentication Code (CMAC)
// specified in the IETF memo "The AES-CMAC Algorithm".

// Returns the most significant bit of a bit string.
#define MSB(x) (x[0] >> 7)

#define initToConstZero(x) \
  do { MEMSET(x, 0x00, 16); } while (0)

#define initToConstRb(x) \
  do { MEMSET(x, 0x00, 15); x[15] = 0x87; } while (0)

/** @brief Computes the Cipher-based Message Authentication Code (CMAC)
 * corresponding to the passed message and the passed key.
 *
 * @param key           The key to be used to produce the cipher-based
 * authentication code.
 *
 * @param message       A pointer to the message for which a cipher-based
 * authentication code should be computed.
 *
 * @param messageLength The length in bytes of the message to be authenticated.
 *
 * @out                 A pointer to a 4-byte area memory in which the resulting
 * cipher-based authentication code will be stored.
 */
void emberAfPluginAesMacAuthenticate(const uint8_t *key,
                                     const uint8_t *message,
                                     uint8_t messageLength,
                                     uint8_t *out);
