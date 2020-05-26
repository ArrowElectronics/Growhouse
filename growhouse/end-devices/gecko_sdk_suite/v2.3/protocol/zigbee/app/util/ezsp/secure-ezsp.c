/*
 * File: secure-ezsp.c
 * Description: Implements secure EZSP functionality.
 *
 * Author(s): Rehan Rasool, rehan.rasool@silabs.com
 *
 * Copyright 2016 by Silicon Labs. All rights reserved.
 */

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "hal/micro/token.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"
#include "app/util/ezsp/serial-interface.h"

#include "app/util/secure-ezsp/802-15-4-ccm.h"
#include "app/util/secure-ezsp/aes.h"

//------------------------------------------------------------------------------
// Static and global variables.

static uint8_t secureEzspState = SECURE_EZSP_STATE_INITIAL;

static EmberKeyData securityKey = { {
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                    } };
static SecureEzspSessionId hostSessionId = { {
                                               0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
                                             } };
static SecureEzspSessionId ncpSessionId = { {
                                              0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB
                                            } };
static SecureEzspSecurityType securityType = SECURE_EZSP_SECURITY_TYPE_TEMPORARY;
static SecureEzspSecurityLevel securityLevel = SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32;

static SecureEzspFrameCounter outgoingFrameCounter = 0x00000000;
static SecureEzspFrameCounter incomingFrameCounterMin = 0x00000000;

#ifdef EZSP_HOST
static SecureEzspFrameCounter incomingAsyncCbFrameCounterMin = 0x00000000;
#else
static SecureEzspFrameCounter outgoingAsyncCbFrameCounter = 0x00000000;
#endif

#if defined(EZSP_UART) || defined(EZSP_USB)
static uint8_t tempBuff[SECURE_EZSP_MAX_FRAME_LENGTH]; // used as a continuous buffer for encryption
EmberMessageBuffer serialGetEzspBuffer(void);
#endif

static void secureEzspFillHeader(uint8_t* headerStartPointer);
static void convertPacketStructureFromEzspToSecureEzsp(void);
static void convertPacketStructureFromSecureEzspToEzsp(void);
static bool verifyIncomingNonce(uint8_t *incomingNonce,
                                SecureEzspFrameCounter incomingFrameCounter);
static void initializeOutgoingNonce(uint8_t *nonce);
static void initializeIncomingNonce(uint8_t *nonce,
                                    SecureEzspFrameCounter incomingFrameCounter);
static void incrementOutgoingCounter(void);
static void incrementIncomingCounter(SecureEzspFrameCounter incomingFrameCounter);

static bool byteCompare(const uint8_t *bytes, uint8_t count, uint8_t target);
static void secureEzspFrameMemMove(uint8_t indexTo, uint8_t indexFrom, uint8_t length);
static void arrayPrint(char* label, uint8_t* array, uint8_t length);

//------------------------------------------------------------------------------
// Internal functions and handlers.

EzspStatus emSecureEzspDeInit(void)
{
  secureEzspState = SECURE_EZSP_STATE_INITIAL;
  outgoingFrameCounter = 0x00000000;
  incomingFrameCounterMin = 0x00000000;
  #ifdef EZSP_HOST
  incomingAsyncCbFrameCounterMin = 0x00000000;
  #else
  outgoingAsyncCbFrameCounter = 0x00000000;
  #endif
  return EZSP_SUCCESS;
}

bool emSecureEzspIsOn(void)
{
  return (secureEzspState == SECURE_EZSP_STATE_SYNCED);
}

bool emSecureEzspParametersArePending(void)
{
  return (secureEzspState == SECURE_EZSP_STATE_PARAMETERS_PENDING);
}

//------------------------------------------------------------------------------
// Before this function:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) | FRAME_ID(1) | PARAMETERS
//
// After this function:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) |
// AUTH_HEADER [CONTROL(1) | SESSION_ID(8) | COUNTER(4) | NONCE(1)]
// ENCRYPTED_PAYLOAD [FRAME_ID(1) | PARAMETERS]
// MIC(32)
//------------------------------------------------------------------------------
EzspStatus emSecureEzspEncode(void)
{
  uint8_t secureEzspStartIndex = EZSP_EXTENDED_FRAME_CONTROL_INDEX + 1;
  uint8_t* secureEzspStartPointer = ezspFrameContents + secureEzspStartIndex;
  uint8_t parametersLength = ezspWritePointer - ezspFrameContents - EZSP_EXTENDED_PARAMETERS_INDEX;

  #ifdef EZSP_HOST
  secureEzspSerialDebug(stderr, "-------- HOST ---- ENCODE \n");
  #else
  secureEzspSerialDebug(stderr, "-------- NCP ----- ENCODE \n");
  #endif

  arrayPrint("EZSP Frame", ezspFrameContents, ezspWritePointer - ezspFrameContents);

  // Convert to Secure EZSP packet structure
  convertPacketStructureFromEzspToSecureEzsp();

  // Fill Secure EZSP header
  secureEzspFillHeader(secureEzspStartPointer);

  uint8_t nonce[SECURE_EZSP_NONCE_SIZE];
  initializeOutgoingNonce(nonce);

  emLoadKeyIntoCore(emberKeyContents(&securityKey));

  // Encrypt content: Frame ID + parameters
  #if defined(EZSP_UART) || defined(EZSP_USB)
  // For UART and USB, we use packet buffers. However, encryption uses
  // continuous frames, so we convert our buffers into a continuous frame
  // Exception: if our frame is going to fit in one buffer, then no need for conversion
  if (((ezspWritePointer - ezspFrameContents) + sizeof(SecureEzspMic))
      <= PACKET_BUFFER_SIZE) {
    emCcmEncryptPacket(nonce,
                       ezspFrameContents,
                       SECURE_EZSP_AUTHENTICATE_SIZE,
                       1 + parametersLength,   // Frame ID + parameters
                       sizeof(SecureEzspMic));
  } else {
    EmberMessageBuffer ezspBuff = serialGetEzspBuffer();
    emberCopyFromLinkedBuffers(ezspBuff,
                               0,
                               tempBuff,
                               ezspWritePointer - ezspFrameContents);
    emCcmEncryptPacket(nonce,
                       tempBuff,
                       SECURE_EZSP_AUTHENTICATE_SIZE,
                       1 + parametersLength,   // Frame ID + parameters
                       sizeof(SecureEzspMic));
    emberCopyToLinkedBuffers(tempBuff,
                             ezspBuff,
                             0,
                             (ezspWritePointer - ezspFrameContents) + sizeof(SecureEzspMic));
  }
  #else
  emCcmEncryptPacket(nonce,
                     ezspFrameContents,
                     SECURE_EZSP_AUTHENTICATE_SIZE,
                     1 + parametersLength,   // Frame ID + parameters
                     sizeof(SecureEzspMic));
  #endif // EZSP_UART || EZSP_USB

  incrementOutgoingCounter();

  ezspWritePointer += sizeof(SecureEzspMic);

  arrayPrint("Secure EZSP Frame", ezspFrameContents, ezspWritePointer - ezspFrameContents);

  secureEzspSerialDebug(stderr, "--------------------- \n");

  return EZSP_SUCCESS;
}

//------------------------------------------------------------------------------
// Before this function:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) |
// AUTH_HEADER [CONTROL(1) | SESSION_ID(8) | COUNTER(4) | NONCE(1)]
// ENCRYPTED_PAYLOAD [FRAME_ID(1) | PARAMETERS]
// MIC(32)
//
// After this function:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) | FRAME_ID(1) | PARAMETERS
//------------------------------------------------------------------------------
EzspStatus emSecureEzspDecode(void)
{
  uint8_t secureEzspStartIndex = EZSP_EXTENDED_FRAME_CONTROL_INDEX + 1;
  uint8_t secureEzspFrameIdIndex = secureEzspStartIndex + SECURE_EZSP_FRAME_ID_INDEX;
  uint8_t secureEzspFrameCounterIndex = secureEzspStartIndex + SECURE_EZSP_FRAME_COUNTER_INDEX;
  uint8_t commandLength = serialGetCommandLength();
  uint8_t parametersLength = commandLength
                             - secureEzspStartIndex
                             - SECURE_EZSP_PARAMETERS_INDEX
                             - sizeof(SecureEzspMic);
  SecureEzspFrameCounter incomingFrameCounter;

  if (commandLength
      < secureEzspStartIndex
      + SECURE_EZSP_PARAMETERS_INDEX
      + sizeof(SecureEzspMic)) {
    return EZSP_DATA_FRAME_TOO_SHORT;
  }

  #ifdef EZSP_HOST
  secureEzspSerialDebug(stderr, "-------- HOST ----- DECODE \n");
  #else
  secureEzspSerialDebug(stderr, "-------- NCP ------ DECODE \n");
  #endif

  arrayPrint("Secure EZSP Frame", ezspFrameContents, commandLength);

  // Extract and store incoming frame counter
  MEMCOPY((uint8_t*)&incomingFrameCounter,
          ezspFrameContents + secureEzspFrameCounterIndex,
          sizeof(SecureEzspFrameCounter));

  if (!verifyIncomingNonce(ezspFrameContents + secureEzspStartIndex,
                           incomingFrameCounter)) {
    return EZSP_ERROR_SECURITY_PARAMETERS_INVALID;
  }

  uint8_t nonce[SECURE_EZSP_NONCE_SIZE];
  initializeIncomingNonce(nonce, incomingFrameCounter);

  emLoadKeyIntoCore(emberKeyContents(&securityKey));

  // Decrypt content: Frame ID + parameters
  #if defined(EZSP_UART) || defined(EZSP_USB)
  // For UART and USB, we use packet buffers. However, encryption uses
  // continuous frames, so we convert our buffers into a continuous frame
  // Exception: if our frame is going to fit in one buffer, then no need for conversion
  if (serialGetCommandLength() <= PACKET_BUFFER_SIZE) {
    if (!emCcmDecryptPacket(nonce,
                            ezspFrameContents,
                            SECURE_EZSP_AUTHENTICATE_SIZE,
                            ezspFrameContents + secureEzspFrameIdIndex,
                            1 + parametersLength,   // Frame ID + parameters
                            sizeof(SecureEzspMic))) {
      return EZSP_ERROR_SECURITY_PARAMETERS_INVALID;
    }
  } else {
    EmberMessageBuffer ezspBuff = serialGetEzspBuffer();
    emberCopyFromLinkedBuffers(ezspBuff,
                               0,
                               tempBuff,
                               serialGetCommandLength());
    if (!emCcmDecryptPacket(nonce,
                            tempBuff,
                            SECURE_EZSP_AUTHENTICATE_SIZE,
                            tempBuff + secureEzspFrameIdIndex,
                            1 + parametersLength,   // Frame ID + parameters
                            sizeof(SecureEzspMic))) {
      return EZSP_ERROR_SECURITY_PARAMETERS_INVALID;
    }
    emberCopyToLinkedBuffers(tempBuff,
                             ezspBuff,
                             0,
                             serialGetCommandLength());
  }
  #else
  if (!emCcmDecryptPacket(nonce,
                          ezspFrameContents,
                          SECURE_EZSP_AUTHENTICATE_SIZE,
                          ezspFrameContents + secureEzspFrameIdIndex,
                          1 + parametersLength,   // Frame ID + parameters
                          sizeof(SecureEzspMic))) {
    return EZSP_ERROR_SECURITY_PARAMETERS_INVALID;
  }
  #endif // EZSP_UART || EZSP_USB

  incrementIncomingCounter(incomingFrameCounter);

  // Convert to EZSP packet structure
  convertPacketStructureFromSecureEzspToEzsp();

  arrayPrint("EZSP Frame", ezspFrameContents, parametersLength + EZSP_EXTENDED_PARAMETERS_INDEX);

  secureEzspSerialDebug(stderr, "--------------------- \n");

  return EZSP_SUCCESS;
}

uint8_t emSecureEzspGetState(void)
{
  return secureEzspState;
}

void emSecureEzspSetState(uint8_t state)
{
  secureEzspState = state;
}

EmberKeyData* emSecureEzspGetSecurityKey(void)
{
  return &securityKey;
}

SecureEzspSecurityType emSecureEzspGetSecurityType(void)
{
  return securityType;
}

void emSecureEzspSetSecurityType(SecureEzspSecurityType type)
{
  securityType = type;
}

void emSecureEzspSetSecurityLevel(SecureEzspSecurityLevel level)
{
  securityLevel = level;
  UNUSED_VAR(securityLevel);
}

// Device A (host) will generate a random 128-bit number (Rand-1) and send it to Device B (NCP)
// Device B (ncp) will generate a random 128-bit number (Rand-2) and send it to Device A (host)
// Both devices will construct a string of bits:  Rand-1 || Rand-2
// Both devices will perform HMAC using the NCP Protocol Security Key using AES-MMO as the underlying hashing algorithm.
// The resulting 16-byte number will be split into two 8-byte values as follows: Result = Session-ID-Host || Session-ID-NCP
void emSecureEzspGenerateSessionIds(SecureEzspRandomNumber *hostRandomNumber,
                                    SecureEzspRandomNumber *ncpRandomNumber)
{
  uint8_t combinedRandomNumbers[SECURE_EZSP_RANDOM_NUMBER_SIZE * 2];
  uint8_t combinedSessionIds[SECURE_EZSP_SESSION_ID_SIZE * 2];
  MEMMOVE(combinedRandomNumbers,
          SecureEzspRandomNumberContents(hostRandomNumber),
          SECURE_EZSP_RANDOM_NUMBER_SIZE);
  MEMMOVE(combinedRandomNumbers + SECURE_EZSP_RANDOM_NUMBER_SIZE,
          SecureEzspRandomNumberContents(ncpRandomNumber),
          SECURE_EZSP_RANDOM_NUMBER_SIZE);

  #ifdef EZSP_HOST
  secureEzspSerialDebug(stderr, "-------- HOST ---- HMAC \n");
  #else
  secureEzspSerialDebug(stderr, "-------- NCP ----- HMAC \n");
  #endif

  arrayPrint("Host Random Number", SecureEzspRandomNumberContents(hostRandomNumber), SECURE_EZSP_RANDOM_NUMBER_SIZE);
  arrayPrint("NCP Random Number", SecureEzspRandomNumberContents(ncpRandomNumber), SECURE_EZSP_RANDOM_NUMBER_SIZE);

  // Perform HMAC here
  emberHmacAesHash(emberKeyContents(&securityKey),
                   combinedRandomNumbers,
                   SECURE_EZSP_RANDOM_NUMBER_SIZE * 2,
                   combinedSessionIds);

  // Split into two 8-byte values
  MEMMOVE(SecureEzspSessionIdContents(&hostSessionId),
          combinedSessionIds,
          SECURE_EZSP_SESSION_ID_SIZE);
  MEMMOVE(SecureEzspSessionIdContents(&ncpSessionId),
          combinedSessionIds + SECURE_EZSP_SESSION_ID_SIZE,
          SECURE_EZSP_SESSION_ID_SIZE);

  arrayPrint("Host Session ID", SecureEzspSessionIdContents(&hostSessionId), SECURE_EZSP_SESSION_ID_SIZE);
  arrayPrint("NCP Session ID", SecureEzspSessionIdContents(&ncpSessionId), SECURE_EZSP_SESSION_ID_SIZE);
}

bool emSecureEzspIsSecurityKeySet(void)
{
  tokTypeSecureEzspSecurityKey tok;
  halCommonGetToken(&tok, TOKEN_SECURE_EZSP_SECURITY_KEY);

  // Token is all 0x00s when not set
  if (!byteCompare(tok.contents, EMBER_ENCRYPTION_KEY_SIZE, 0x00)) {
    arrayPrint("security key", tok.contents, EMBER_ENCRYPTION_KEY_SIZE);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
// Static functions

//------------------------------------------------------------------------------
// Extended EZSP packet structure:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) | FRAME_ID(1) | PARAMETERS
//
// Secure EZSP packet structure:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) |
// AUTH_HEADER [CONTROL(1) | SESSION_ID(8) | COUNTER(4) | NONCE(1) |
// ENCRYPTED_PAYLOAD [FRAME_ID(1) | PARAMETERS]
// MIC(32)
//------------------------------------------------------------------------------
static void convertPacketStructureFromEzspToSecureEzsp(void)
{
  // initialize variables needed for changing packet structure
  uint8_t* secureEzspStartPointer = ezspFrameContents + EZSP_EXTENDED_FRAME_CONTROL_INDEX + 1;
  uint8_t secureEzspStartIndex = EZSP_EXTENDED_FRAME_CONTROL_INDEX + 1;
  uint8_t secureEzspParametersIndex = secureEzspStartIndex + SECURE_EZSP_PARAMETERS_INDEX;
  uint8_t parametersLength = ezspWritePointer - ezspFrameContents - EZSP_EXTENDED_PARAMETERS_INDEX;

  // Move parameters to Secure packet format
  secureEzspFrameMemMove(secureEzspParametersIndex,
                         EZSP_EXTENDED_PARAMETERS_INDEX,
                         parametersLength);

  // Move Frame ID
  secureEzspStartPointer[SECURE_EZSP_FRAME_ID_INDEX] = ezspFrameContents[EZSP_EXTENDED_FRAME_ID_INDEX];
}

//------------------------------------------------------------------------------
// Secure EZSP packet structure:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) |
// AUTH_HEADER [CONTROL(1) | SESSION_ID(8) | COUNTER(4) | NONCE(1) |
// ENCRYPTED_PAYLOAD [FRAME_ID(1) | PARAMETERS]
// MIC(32)
//
// Extended EZSP packet structure:
// SEQ(1) | FC(1) | FRAME_ID(0xFF) | EXTENDED_FC(1) | FRAME_ID(1) | PARAMETERS
//------------------------------------------------------------------------------
static void convertPacketStructureFromSecureEzspToEzsp(void)
{
  // initialize variables needed for changing packet structure
  uint8_t* secureEzspStartPointer = ezspFrameContents + EZSP_EXTENDED_FRAME_CONTROL_INDEX + 1;
  uint8_t secureEzspStartIndex = EZSP_EXTENDED_FRAME_CONTROL_INDEX + 1;
  uint8_t secureEzspParametersIndex = secureEzspStartIndex + SECURE_EZSP_PARAMETERS_INDEX;
  uint8_t parametersLength = serialGetCommandLength()
                             - EZSP_EXTENDED_FRAME_CONTROL_INDEX - 1
                             - SECURE_EZSP_PARAMETERS_INDEX
                             - sizeof(SecureEzspMic);

  // Move Frame ID
  ezspFrameContents[EZSP_EXTENDED_FRAME_ID_INDEX] = secureEzspStartPointer[SECURE_EZSP_FRAME_ID_INDEX];

  // Move parameters to standard packet format
  secureEzspFrameMemMove(EZSP_EXTENDED_PARAMETERS_INDEX,
                         secureEzspParametersIndex,
                         parametersLength);
}

static void secureEzspFillHeader(uint8_t* headerStartPointer)
{
  #ifdef EZSP_HOST
  SecureEzspSessionId* sessionId = &hostSessionId;
  SecureEzspFrameCounter* outgoingCounter = &outgoingFrameCounter;
  #else
  SecureEzspSessionId* sessionId = &ncpSessionId;
  SecureEzspFrameCounter* outgoingCounter;
  if ((serialGetResponseByte(EZSP_FRAME_CONTROL_INDEX) & EZSP_FRAME_CONTROL_ASYNCH_CB_MASK)
      == EZSP_FRAME_CONTROL_ASYNCH_CB) {
    outgoingCounter = &outgoingAsyncCbFrameCounter;
  } else {
    outgoingCounter = &outgoingFrameCounter;
  }
  #endif

  // SECURE_EZSP_FRAME_CONTROL_INDEX
  *(headerStartPointer + SECURE_EZSP_FRAME_CONTROL_INDEX) = 0;

  // SECURE_EZSP_SESSION_ID_INDEX
  MEMCOPY(headerStartPointer + SECURE_EZSP_SESSION_ID_INDEX,
          SecureEzspSessionIdContents(sessionId),
          SECURE_EZSP_SESSION_ID_SIZE);

  // SECURE_EZSP_FRAME_COUNTER_INDEX
  MEMCOPY(headerStartPointer + SECURE_EZSP_FRAME_COUNTER_INDEX,
          (uint8_t*)outgoingCounter,
          sizeof(SecureEzspFrameCounter));

  // SECURE_EZSP_NONCE_SECURITY_LEVEL_INDEX
  *(headerStartPointer + SECURE_EZSP_NONCE_SECURITY_LEVEL_INDEX) = 0x05;

  ezspWritePointer += (SECURE_EZSP_PARAMETERS_INDEX - 1);
}

static void arrayPrint(char* label, uint8_t* array, uint8_t length)
{
  secureEzspSerialDebug(stderr, "%s [", label);
  for (uint8_t i = 0; i < length; i++) {
    secureEzspSerialDebug(stderr, " %02X", array[i]);
  }
  secureEzspSerialDebug(stderr, " ]\n");
}

static bool byteCompare(const uint8_t *bytes, uint8_t count, uint8_t target)
{
  uint8_t i;
  for (i = 0; i < count; i++, bytes++) {
    if (*bytes != target) {
      return false;
    }
  }
  return true;
}

static void secureEzspFrameMemMove(uint8_t indexTo, uint8_t indexFrom, uint8_t length)
{
  #if defined(EZSP_UART) || defined(EZSP_USB)
  EmberMessageBuffer ezspBuff = serialGetEzspBuffer();
  emberMoveBufferBytes(ezspBuff, indexTo, ezspBuff, indexFrom, length);
  #else
  MEMMOVE(ezspFrameContents + indexTo, ezspFrameContents + indexFrom, length);
  #endif
}

static bool verifyIncomingNonce(uint8_t *incomingNonce,
                                SecureEzspFrameCounter incomingFrameCounter)
{
  #ifdef EZSP_HOST
  SecureEzspSessionId* sessionId = &ncpSessionId;
  SecureEzspFrameCounter incomingCounterMin;
  if ((serialGetResponseByte(EZSP_FRAME_CONTROL_INDEX) & EZSP_FRAME_CONTROL_ASYNCH_CB_MASK)
      == EZSP_FRAME_CONTROL_ASYNCH_CB) {
    incomingCounterMin = incomingAsyncCbFrameCounterMin;
  } else {
    incomingCounterMin = incomingFrameCounterMin;
  }
  #else
  SecureEzspSessionId* sessionId = &hostSessionId;
  SecureEzspFrameCounter incomingCounterMin = incomingFrameCounterMin;
  #endif

  // verify frame control - this is unused right now
  if (*(incomingNonce + SECURE_EZSP_FRAME_CONTROL_INDEX) != 0x00) {
    return false;
  }

  // verify session id
  if (memcmp(incomingNonce + SECURE_EZSP_SESSION_ID_INDEX,
             SecureEzspSessionIdContents(sessionId),
             EUI64_SIZE)) {
    return false;
  }

  // verify frame counter - this should not be less than previously received value
  if (incomingFrameCounter < incomingCounterMin) {
    return false;
  }

  // verify security level - only 0x05 is currently supported
  if (*(incomingNonce + SECURE_EZSP_NONCE_SECURITY_LEVEL_INDEX)
      != SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32) {
    return false;
  }

  return true;
}

static void initializeOutgoingNonce(uint8_t *nonce)
{
  #ifdef EZSP_HOST
  SecureEzspSessionId* sessionId = &hostSessionId;
  SecureEzspFrameCounter* outgoingCounter = &outgoingFrameCounter;
  #else
  SecureEzspSessionId* sessionId = &ncpSessionId;
  SecureEzspFrameCounter* outgoingCounter;
  if ((serialGetResponseByte(EZSP_FRAME_CONTROL_INDEX) & EZSP_FRAME_CONTROL_ASYNCH_CB_MASK)
      == EZSP_FRAME_CONTROL_ASYNCH_CB) {
    outgoingCounter = &outgoingAsyncCbFrameCounter;
  } else {
    outgoingCounter = &outgoingFrameCounter;
  }
  #endif

  nonce[SECURE_EZSP_FRAME_CONTROL_INDEX] = 0x00;

  MEMCOPY(nonce + SECURE_EZSP_SESSION_ID_INDEX,
          SecureEzspSessionIdContents(sessionId),
          EUI64_SIZE);

  MEMCOPY(nonce + SECURE_EZSP_FRAME_COUNTER_INDEX,
          (uint8_t*)outgoingCounter,
          sizeof(SecureEzspFrameCounter));

  nonce[SECURE_EZSP_NONCE_SECURITY_LEVEL_INDEX] = SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32;
}

static void initializeIncomingNonce(uint8_t *nonce,
                                    SecureEzspFrameCounter incomingFrameCounter)
{
  #ifdef EZSP_HOST
  SecureEzspSessionId* sessionId = &ncpSessionId;
  #else
  SecureEzspSessionId* sessionId = &hostSessionId;
  #endif

  nonce[SECURE_EZSP_FRAME_CONTROL_INDEX] = 0x00;

  MEMCOPY(nonce + SECURE_EZSP_SESSION_ID_INDEX,
          SecureEzspSessionIdContents(sessionId),
          EUI64_SIZE);

  MEMCOPY(nonce + SECURE_EZSP_FRAME_COUNTER_INDEX,
          (uint8_t*)&incomingFrameCounter,
          sizeof(SecureEzspFrameCounter));

  nonce[SECURE_EZSP_NONCE_SECURITY_LEVEL_INDEX] = SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32;
}

static void incrementOutgoingCounter(void)
{
  #ifdef EZSP_HOST
  outgoingFrameCounter++;
  #else
  if ((serialGetResponseByte(EZSP_FRAME_CONTROL_INDEX) & EZSP_FRAME_CONTROL_ASYNCH_CB_MASK)
      == EZSP_FRAME_CONTROL_ASYNCH_CB) {
    outgoingAsyncCbFrameCounter++;
  } else {
    outgoingFrameCounter++;
  }
  #endif
}

static void incrementIncomingCounter(SecureEzspFrameCounter incomingFrameCounter)
{
  #ifdef EZSP_HOST
  if ((serialGetResponseByte(EZSP_FRAME_CONTROL_INDEX) & EZSP_FRAME_CONTROL_ASYNCH_CB_MASK)
      == EZSP_FRAME_CONTROL_ASYNCH_CB) {
    incomingAsyncCbFrameCounterMin = incomingFrameCounter;
  } else {
    incomingFrameCounterMin = incomingFrameCounter;
  }
  #else
  incomingFrameCounterMin = incomingFrameCounter;
  #endif
}
