// File: zll-token-config.h
//
// Description: ZigBee Light Link token definitions used by the stack.
//
// Copyright 2011 by Ember Corporation.  All rights reserved.               *80*

#if defined(DEFINETYPES)

typedef struct {
  uint32_t bitmask;
  uint16_t freeNodeIdMin;
  uint16_t freeNodeIdMax;
  uint16_t myGroupIdMin;
  uint16_t freeGroupIdMin;
  uint16_t freeGroupIdMax;
  uint8_t rssiCorrection;
} tokTypeStackZllData;

typedef struct {
  uint32_t bitmask;
  uint8_t keyIndex;
  uint8_t encryptionKey[16];
  uint8_t preconfiguredKey[16];
} tokTypeStackZllSecurity;

#endif //DEFINETYPES

#ifdef DEFINETOKENS

DEFINE_BASIC_TOKEN(STACK_ZLL_DATA,
                   tokTypeStackZllData,
{
  EMBER_ZLL_STATE_FACTORY_NEW,                    // bitmask
  0x0000,                                         // freeNodeIdMin
  0x0000,                                         // freeNodeIdMax
  0x0000,                                         // myGroupIdMin
  0x0000,                                         // freeGroupIdMin
  0x0000,                                         // freeGroupIdMax
  0                                               // rssiCorrection
})

DEFINE_BASIC_TOKEN(STACK_ZLL_SECURITY,
                   tokTypeStackZllSecurity,
{
  0x00000000,
  0x00,
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
})

#endif
