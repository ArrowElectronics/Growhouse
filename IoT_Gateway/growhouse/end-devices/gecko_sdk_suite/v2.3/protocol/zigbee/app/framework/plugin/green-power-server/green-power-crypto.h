#ifndef SILABS_PLUGIN_GREEN_POWER_CRYPTO_H
#define SILABS_PLUGIN_GREEN_POWER_CRYPTO_H

// A nonce contains the following fields:
#define STANDALONE_FLAGS_INDEX                   0
#define STANDALONE_NONCE_SOURCE_ADDR_INDEX       1
#define STANDALONE_NONCE_FRAME_COUNTER_INDEX     9
#define STANDALONE_NONCE_SECURITY_CONTROL_INDEX 13
#define STANDALONE_VARIABLE_FIELD_INDEX_HIGH    14
#define STANDALONE_VARIABLE_FIELD_INDEX_LOW     15

#define FRAME_COUNTER_UPDATE_INTERVAL_LOG 12
#define FRAME_COUNTER_UPDATE_INTERVAL  (1 << FRAME_COUNTER_UPDATE_INTERVAL_LOG)
#define FRAME_COUNTER_UPDATE_MASK      (FRAME_COUNTER_UPDATE_INTERVAL - 1)

#define SECURITY_BLOCK_SIZE 16

void emCalculateMic(EmberMessageBuffer header,
                    uint8_t *contents,
                    uint8_t authenticationStartIndex,
                    uint8_t encryptionStartIndex,
                    uint8_t packetLength,
                    bool useCbcMac,
                    uint8_t nonce[SECURITY_BLOCK_SIZE],
                    uint8_t micResult[]);

void emEncryptBytes(uint8_t* bytes,
                    uint8_t length,
                    uint16_t blockCount,
                    uint8_t nonce[SECURITY_BLOCK_SIZE]);

void emEncryptPayload(EmberMessageBuffer header,
                      uint8_t encryptionStartIndex,
                      uint8_t nonce[SECURITY_BLOCK_SIZE]);

// The payload gets encrypted starting from block 1.
#define emEncryptFlatPayload(payload, length, nonce) \
  (emEncryptBytes((payload), (length), 1, (nonce)))

// Routines for extracting timing data for the CCM* crypto operations.
// The code currently only measures NWK encryption.  Since we are only
// measuring CCM* performance: MIC create/check, Encrypt/Decrypt,
// and not the overall packet manipulation we don't bother to measure
// APS encryption.  It is the same as NWK but the max packet size we can
// operate over is smaller.

// We store separate results for sending and receiving
#define NUMBER_RESULTS 10

typedef struct {
  uint32_t micTimingMicroSeconds;
  uint8_t micPacketSize;
  uint32_t encryptDecryptTimingMicroSeconds;
  uint8_t encryptDecryptPacketSize;
  uint32_t frameCounter;
} CryptoTimingInfo;

void emCryptoTimingRecordEnable(bool on);
void emCryptoTimingReset(void);
const CryptoTimingInfo* emCryptoTimingGetData(uint8_t index,
                                              bool outgoing);

// DON'T ADD Below here.  Add new stuff above the CCM* timing header block above.

#endif // SILABS_PLUGIN_GREEN_POWER_CRYPTO_H
