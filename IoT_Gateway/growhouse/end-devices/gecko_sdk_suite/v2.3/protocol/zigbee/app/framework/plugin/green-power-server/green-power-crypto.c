#include "core/ember-stack.h"
#include "core/ember-multi-network.h"
#include "hal/hal.h"
#include "include/error.h"
#include "phy/security.h"
#include "stack/platform/micro/aes.h"
#include "phy/ieee802154mac.h"
#include "zigbee/aps-security.h"
#include "zigbee/aps-keys.h"

// Common Security code that is platform agnostic.  Originally
// this file was named security.c, but due to restrictions with how
// our jam files are setup we can't have any 2 compiled object files
// that are the same name.

#include "include/packet-buffer.h"
#include "framework/packet-header.h"
#include "framework/zigbee-packet-header.h"

#include "stack/config/token-cache.h"

#if defined EMBER_TEST
  #include "phy/simulation/security.h"
#endif

#include "green-power-crypto.h"

// This implements the CCM* encryption algorithm, using AES-128 as the
// underlying block cipher.  AES-128 is essentially a hash function
// controlled by 128-bit (16 byte) keys.  The AES-128 algorithm takes a key
// and a 16-byte block of data and produces a 16-byte block of noise.
//
// The em250 implements AES-128 in hardware.  The hardware can also handle
// some of the other CCM* calculations, but at this point we only use the
// basic AES-128 functionality.
//
// CCM* uses AES-128 for authentication and encryption.  In authentication,
// AES-128 is used create a hash number from a message, called a MIC.
// The MIC is appended to the message before transmission.  The recipient
// hashes the message and verifies that it obtains the same MIC.  Changes
// to either the body of the message or the appended MIC will result in
// the recipient's MIC differing from the appended MIC.
//
// For encryption, AES-128 is used to create a series of pseudo-random
// blocks which are xor-ed with the message to produce the encrypted
// version.  The recipient performs the same operation, which results in
// the original message.  It is important that each message be encrypted
// with a different psuedo-random sequence.  If two messages used the
// same sequence, xor-ing the encrypted messages would produce the xor
// of the unencrypted messages.
//
// Both authentication and encryption begin by encrypting a special block
// of data called a 'nonce' (as in 'for the nonce').  The goal is that each
// use of a particular key will use a different nonce.  The nonce is:
//
// Offset Size
//   0     1    flags
//   1     8    source EUI64 *
//   9     4    frame counter
//  13     1    control byte
//  14     2    block count
//
// * The APS Auxiliary Frame has no Source EUI64 field, so the spec says
//   the value that is used in the nonce for APS Security is the destination's
//   EUI64.
//
// The frame counter is incremented for each message and the block count is
// incremented for each encryption block.  The flags and control byte ensure
// that different operations on the same message, such as MIC generation and
// encryption, do not use the same nonce.

// Handy macro.  This is unsafe because it duplicates its arguments.
#define min(x, y) ((x) < (y) ? (x) : (y))

// phy/security.h API
uint32_t emSecurityNonceFrameCounter;

typedef enum {
  SENDING_PACKET      = 0,
  RECEIVING_PACKET    = 1,
} CryptoPacketType;
#define NUMBER_OPERATIONS 2

#define MIC_OPERATION             true
#define ENCRYPT_DECRYPT_OPERATION false

//----------------------------------------------------------------
// MIC encryption flags

// For authentication, the flags byte has the following form:
//  <reserved:1> <adata:1> <M:3> <L:3>
// Where
//  <reserved:1> = 0   Reserved for future expansion.
//  <adata:1>          0 for zero-length messages, 1 for non-zero-length
//  <M:3>              MIC length, encoded as (micLength - 2)/2
//  <L:3>              Number of bytes used to represent the length
//                     of the message, - 1.

#define STANDALONE_FLAGS_ADATA_FIELD_NONZERO 0x40
#define STANDALONE_FLAGS_M_FIELD_4_BYTES     0x08
#define STANDALONE_FLAGS_M_FIELD_8_BYTES     0x18
#define STANDALONE_FLAGS_M_FIELD_16_BYTES    0x38
#define STANDALONE_FLAGS_L_FIELD             0x01
#define STANDALONE_FLAGS_ENCRYPTION_FLAGS    0x01

#define COMMON_MIC_FLAGS \
  (STANDALONE_FLAGS_ADATA_FIELD_NONZERO | STANDALONE_FLAGS_L_FIELD)

// Handy arrays that will be needed if we ever support anything other than
// level 5.
//
//static uint8_t PGM micLengthValues[] = { 0, 4, 8, 16, 4 };
//static uint8_t PGM micFlagValues[] =
//  {
//    0,
//    COMMON_MIC_FLAGS | STANDALONE_FLAGS_M_FIELD_4_BYTES,
//    COMMON_MIC_FLAGS | STANDALONE_FLAGS_M_FIELD_8_BYTES,
//    COMMON_MIC_FLAGS | STANDALONE_FLAGS_M_FIELD_16_BYTES,
//    STANDALONE_FLAGS_L_FIELD | STANDALONE_FLAGS_M_FIELD_4_BYTES // C4 MIC
//  };

#define micLength() 4

//----------------------------------------------------------------

//----------------------------------------------------------------
// Performs an actual nonce encryption, after first setting the fields
// specific to this block.  We do a copy to avoid clobbering the (shared)
// nonce.

static void encryptNonce(uint8_t nonce[SECURITY_BLOCK_SIZE],
                         uint8_t flags,
                         uint16_t variableField,
                         uint8_t block[SECURITY_BLOCK_SIZE])
{
  MEMMOVE(block, nonce, SECURITY_BLOCK_SIZE);

  block[STANDALONE_FLAGS_INDEX] = flags;
  block[STANDALONE_VARIABLE_FIELD_INDEX_HIGH] = HIGH_BYTE(variableField);
  block[STANDALONE_VARIABLE_FIELD_INDEX_LOW] = LOW_BYTE(variableField);

  emStandAloneEncryptBlock(block);
}

//----------------------------------------------------------------
// This performs the core of the MIC calculation.  'Count' bytes from 'buffer'
// (or 'contents') are xor-ed into 'block' and then encrypted.  We start at
// 'blockIndex' in the block and 'bufferIndex' in the buffer (or 'contents').
// If 'contents' is non-NULL the bytes are read from their rather than from
// 'buffer'.
//
// The final blockIndex is returned.

static uint8_t xorBufferBytesIntoBlock(uint8_t *block,
                                       uint8_t blockIndex,
                                       EmberMessageBuffer buffer,
                                       uint8_t *contents,
                                       uint8_t bufferIndex,
                                       uint8_t count)
{
  uint8_t i;

  for (i = 0; i < count; ) {
    uint8_t needed = SECURITY_BLOCK_SIZE - blockIndex;
    uint8_t todo = count - i;
    uint8_t copied = min(todo, needed);
    uint8_t *bytes;
    uint8_t j;

    bytes = contents + bufferIndex + i;

    for (j = 0; j < copied; j++, blockIndex++) {
      block[blockIndex] ^= *bytes++;
    }
    i += copied;

    if (blockIndex == SECURITY_BLOCK_SIZE) {
      emStandAloneEncryptBlock(block);
      blockIndex = 0;
    }
  }
  return blockIndex;
}

//----------------------------------------------------------------
// This performs the core of the MIC calculation.  'Count' bytes from 'buffer'
// (or 'contents') are xor-ed into 'block' and then encrypted.  We start at
// 'blockIndex' in the block and 'bufferIndex' in the buffer (or 'contents').
// If 'contents' is non-NULL the bytes are read from their rather than from
// 'buffer'.
//
// The final blockIndex is returned.

// Calculate the MIC by hashing first the authenticated portion of the
// packet and then the encrypted portion (which hasn't been encrypted yet).
//
// If 'contents' is NULL then the packet is in 'header' and the encrypted
// bytes may be split between the header and payload.  If 'contents' is
// not NULL then it points to contents of the packet and 'packetLength'
// is the length of the packet.
//
// The encrypted bytes are processed on a block boundary, so we finish off
// the block at the end of the authenticated bytes.
//
// The 'for' loop goes around three times for buffers (authenticated bytes,
// encrypted bytes in the header, encrypted bytes in the payload) and two
// times for flat packets (authenticated bytes, encrypted bytes).

void emCalculateMic(EmberMessageBuffer header,
                    uint8_t *contents,
                    uint8_t authenticationStartIndex,
                    uint8_t encryptionStartIndex,
                    uint8_t packetLength,
                    bool useCbcMac,
                    uint8_t nonce[SECURITY_BLOCK_SIZE],
                    uint8_t micResult[MIC_SIZE_MAX])
{
  uint8_t encryptionBlock[SECURITY_BLOCK_SIZE];
  uint8_t blockIndex = useCbcMac ? 1 : 2; // skip over length
  uint8_t packetIndex = authenticationStartIndex;
  uint8_t todo = encryptionStartIndex - authenticationStartIndex;
  uint8_t phase;

  encryptNonce(nonce,
               (useCbcMac
                ? STANDALONE_FLAGS_L_FIELD | STANDALONE_FLAGS_M_FIELD_4_BYTES
                : COMMON_MIC_FLAGS | STANDALONE_FLAGS_M_FIELD_4_BYTES),
               packetLength - encryptionStartIndex,    // encryption length
               encryptionBlock);

  // First two bytes are the 16-bit representation of the frame length,
  // high byte first.  The high byte is zero, so no XOR is actually
  // needed.
  // encryptionBlock[0] ^= HIGH_BYTE(authenticationLength);
  encryptionBlock[1] ^= encryptionStartIndex - authenticationStartIndex;

  if (useCbcMac) {
    // Start with zeroes so that first block XOR is a no-op.
    MEMSET(encryptionBlock, 0, SECURITY_BLOCK_SIZE);
    // CBC-MAC with 2420 HEAD option encodes auth length on first byte.
    encryptionBlock[0] = encryptionStartIndex - authenticationStartIndex;
  }

  // phase 0: authenticated bytes (all in header in CCM*)
  // phase 1: encrypted bytes in header (doesn't exist in CBC-MAC)
  // phase 2: encrypted bytes in payload (doesn't exist in CBC-MAC)

  for (phase = 0; phase < 3; phase++) {
    blockIndex = xorBufferBytesIntoBlock(encryptionBlock,
                                         blockIndex,
                                         0xFF,
                                         contents,
                                         packetIndex,
                                         todo);
    packetIndex += todo;

    if (phase != 1      // still have payload to do
        && 0 < blockIndex) {
//      simPrint("encrypt block");
//      printNonce("block", encryptionBlock);
      emStandAloneEncryptBlock(encryptionBlock);
      blockIndex = 0;
    }

    if (phase == 0) {    // next is encrypted portion
      phase += 1;               // no payload buffer to do
      todo = packetLength - encryptionStartIndex;
    }
  }

  MEMMOVE(micResult, encryptionBlock, micLength());
}

// Encrypt the payload by xor-ing it with a series of AES-encrypted nonces.

// Same function, but here the bytes are in an array instead of in a buffer.
// Much simpler.

void emEncryptBytes(uint8_t* bytes,
                    uint8_t length,
                    uint16_t blockCount,
                    uint8_t nonce[SECURITY_BLOCK_SIZE])
{
  uint8_t encryptionBlock[SECURITY_BLOCK_SIZE];

  for (; 0 < length; ) {
    uint8_t todo = min(length, SECURITY_BLOCK_SIZE);
    uint8_t i;

    encryptNonce(nonce,
                 STANDALONE_FLAGS_ENCRYPTION_FLAGS,
                 blockCount,
                 encryptionBlock);
    blockCount += 1;

    for (i = 0; i < todo; i++) {
      *bytes++ ^= encryptionBlock[i];
    }

    length -= todo;
  }
}

// The MIC gets encrypted as block zero of the message.

#define encryptMic(mic, nonce) \
  (emEncryptBytes((mic), micLength(), 0, (nonce)))

//----------------------------------------------------------------
// Initialize the nonce from an aux frame.  This returns the size of
// the aux frame.

//----------------------------------------------------------------

//----------------------------------------------------------------
// Decryption.
