/*
 * File: gp-security.c
 * Description: GP security code
 *
 * Author(s): Jeffrey Rosenberger, jeffrey.rosenberger@silabs.com
 *
 * Copyright 2015 Silicon Laboratories, Inc.                                *80*
 */

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"

#include "green-power-crypto.h"

extern void emLoadKeyIntoCore(const uint8_t* key);

#define EMBER_GP_SECURITY_MIC_LENGTH 4
#define SECURITY_BLOCK_SIZE 16

// A nonce contains the following fields:
#define STANDALONE_FLAGS_INDEX                   0
#define STANDALONE_NONCE_SOURCE_ADDR_INDEX       1
#define STANDALONE_NONCE_FRAME_COUNTER_INDEX     9
#define STANDALONE_NONCE_SECURITY_CONTROL_INDEX 13

#define encryptMic(mic, nonce) \
  (emEncryptBytes((mic), EMBER_GP_SECURITY_MIC_LENGTH, 0, (nonce)))

bool emDecryptKey(uint8_t appId,
                  uint32_t gpdSrcId,
                  uint8_t *gpdIeee,
                  const EmberKeyData* linkKey,
                  uint8_t mic[4],
                  EmberKeyData* key)
{
  uint8_t temparray[20];
  uint8_t nonce[SECURITY_BLOCK_SIZE];
  nonce[0] = 0x49;
  // Source IEEE address.
  if (appId == 0x02) {
    MEMMOVE(nonce + 1,
            gpdIeee,
            EUI64_SIZE);
    MEMMOVE(nonce + STANDALONE_NONCE_FRAME_COUNTER_INDEX, gpdIeee, 4);
    MEMMOVE(temparray, gpdIeee, 4);
  } else {
    temparray[0] = BYTE_0(gpdSrcId);
    temparray[0 + 1] = BYTE_1(gpdSrcId);
    temparray[0 + 2] = BYTE_2(gpdSrcId);
    temparray[0 + 3] = BYTE_3(gpdSrcId);
    //MEMMOVE(nonce+1,&(gpdSrcId),4);
    //MEMMOVE(nonce+5,&(gpdSrcId),4);
    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX] = BYTE_0(gpdSrcId);
    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 1] = BYTE_1(gpdSrcId);
    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 2] = BYTE_2(gpdSrcId);
    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 3] = BYTE_3(gpdSrcId);

    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 4] = BYTE_0(gpdSrcId);
    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 5] = BYTE_1(gpdSrcId);
    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 6] = BYTE_2(gpdSrcId);
    nonce[STANDALONE_NONCE_SOURCE_ADDR_INDEX + 7] = BYTE_3(gpdSrcId);

    // Frame counter.
    nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX] = BYTE_0(gpdSrcId);
    nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX + 1] = BYTE_1(gpdSrcId);
    nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX + 2] = BYTE_2(gpdSrcId);
    nonce[STANDALONE_NONCE_FRAME_COUNTER_INDEX + 3] = BYTE_3(gpdSrcId);

    // Security level.
  }
  nonce[STANDALONE_NONCE_SECURITY_CONTROL_INDEX] = 0x05; //XXXJAR true for incoming, appid 2 outgoing is 0xC5 check this
  MEMMOVE(temparray + 4, key->contents, EMBER_ENCRYPTION_KEY_SIZE);
  emLoadKeyIntoCore(linkKey->contents);
  emEncryptBytes(key->contents, EMBER_ENCRYPTION_KEY_SIZE, 1, nonce);
  MEMCOPY(temparray + 4, key->contents, EMBER_ENCRYPTION_KEY_SIZE);
  emCalculateMic(0xFF,//EMBER_NULL_MESSAGE_BUFFER,
                 temparray,
                 0,
                 4,
                 20,
                 false,
                 nonce,
                 mic);
  //int i;
  //emberAfGreenPowerClusterPrintln("pre-calced MIC");
  //for (i = 0; i < 4; i++)
  //  emberAfGreenPowerClusterPrintln("%x",mic[i]);
  encryptMic(mic, nonce);;
  //emberAfGreenPowerClusterPrintln("\ncalced MIC");
  //for (i = 0; i < 4; i++)
  //  emberAfGreenPowerClusterPrintln("%x",mic[i]);;
  //for (i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++)
  //  emberAfCorePrint("%x",key->contents[i]);

  return true;  //TODO: check the MIC
}
