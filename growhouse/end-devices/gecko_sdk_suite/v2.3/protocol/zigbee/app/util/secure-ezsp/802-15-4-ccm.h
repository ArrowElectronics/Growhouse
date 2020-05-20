/*
 * File: 802.15.4-ccm.h
 * Description: CCM* as used by 802.15.4 security.
 *
 * Copyright 2017 Silicon Laboratories, Inc.                                *80*
 */

// Offsets into the initial block.  The code calls this block the
// nonce.  Strictly speaking, the nonce is the source address, frame
// counter, and security control.  It does not include the flags and
// the variable field (which is used to encode the length of the
// message.

#define STANDALONE_FLAGS_INDEX                   0
#define STANDALONE_NONCE_SOURCE_ADDR_INDEX       1
#define STANDALONE_NONCE_FRAME_COUNTER_INDEX     9
#define STANDALONE_NONCE_SECURITY_CONTROL_INDEX 13
#define STANDALONE_VARIABLE_FIELD_INDEX_HIGH    14
#define STANDALONE_VARIABLE_FIELD_INDEX_LOW     15

// The core encryption function.
void emCcmEncrypt(const uint8_t *nonce,
                  uint8_t *authenticate,
                  uint16_t authenticateLength,
                  uint8_t *encrypt,
                  uint16_t encryptLength,
                  uint8_t *mic,
                  uint8_t packetMicLength);

// // Wrapper used when the authenticated, encrypted, and mic sections are
// // adjacent.
void emCcmEncryptPacket(const uint8_t *nonce,
                        uint8_t *packet,
                        uint16_t authenticateLength,
                        uint16_t encryptLength,
                        uint8_t micLength);

// // Decrypts the packet in place.  The security subframe is removed.
// //
// // Returns true if successful and false otherwise.

bool emCcmDecryptPacket(const uint8_t *nonce,
                        uint8_t *packet,
                        uint16_t authenticateLength,
                        uint8_t *encrypt,
                        uint16_t encryptLength,
                        uint8_t packetMicLength);
