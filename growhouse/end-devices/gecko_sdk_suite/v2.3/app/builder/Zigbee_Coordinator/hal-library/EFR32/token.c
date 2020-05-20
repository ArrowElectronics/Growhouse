/** @file hal/micro/cortexm3/efm32/token.c
 * @brief Token implementation.
 * See token.h for details.
 *
 * <!-- Copyright 2014 Silicon Laboratories, Inc.                        *80*-->
 */
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "stack/include/error.h"
#include "hal/hal.h"
#include "serial/serial.h"

#ifndef SER232
  #define SER232 0
#endif

//prints debug data from the token access functions
#define TOKENDBG(x)
//#define TOKENDBG(x) x

#ifdef USE_NVM3

#include "hal/plugin/nvm3/nvm3-token.h"

// Global variable used by library code to know number of tokens
#ifdef SIMEE2_TO_NVM3_UPGRADE
uint8_t tokenCount = TOKEN_COUNT;
#endif

EmberStatus halSimEeToNvm3Upgrade(void);

static bool tokensActive = false;

EmberStatus halStackInitTokens(void)
{
  uint8_t i;
  uint8_t arrayIndex;
  size_t size;
  uint32_t objectType;
  Ecode_t ecode, status;
  EmberStatus ret;

  if (halSimEeToNvm3Upgrade()) {
    return (EmberStatus) EMBER_NVM3_ERR_UPGRADE;
  }

  ecode = nvm3_open(nvm3_defaultHandle, nvm3_defaultInit);
  TOKENDBG(emberSerialPrintf(SER232,
                             "halStackInitTokens nvm3_open status: 0x%x\r\n",
                             ecode); )

  for (i = 0; i < (uint8_t) TOKEN_COUNT; i++) {
    if (ecode == ECODE_NVM3_OK) {
      TOKENDBG(emberSerialPrintf(SER232,
                                 "Token[%d]: Key=0x%4x Array size =%d \r\n",
                                 i,
                                 tokenNvm3Keys[i],
                                 tokenArraySize[i]); )

      if (tokenIsCnt[i]) { // Counter token
        status = nvm3_getObjectInfo(nvm3_defaultHandle,
                                    tokenNvm3Keys[i],
                                    &objectType,
                                    &size);

        // Create NVM3 objects for tokens if they don't exist already
        // or if type or size doesn't match
        if ((status == ECODE_NVM3_ERR_KEY_NOT_FOUND)
            || ((status == ECODE_NVM3_OK)
                && (objectType != NVM3_OBJECTTYPE_COUNTER))) {
          TOKENDBG(emberSerialPrintf(SER232, "Creating counter token from default value\r\n"); )

          // If object type is wrong we need to delete it before creating the
          // new one with the correct type.
          if (status == ECODE_NVM3_OK) {
            (void) nvm3_deleteObject(nvm3_defaultHandle, tokenNvm3Keys[i]);
            TOKENDBG(emberSerialPrintf(SER232, "Deleting object of wrong type\r\n"); )
          }

          ecode = nvm3_writeCounter(nvm3_defaultHandle,
                                    tokenNvm3Keys[i],
                                    *(const uint32_t *) tokenDefaults[i]);
          TOKENDBG(emberSerialPrintf(SER232,
                                     "halStackInitTokens nvm3_writeCounter value: 0x%4x status: 0x%x\r\n",
                                     *(uint32_t *) tokenDefaults[i],
                                     ecode); )
        }
      } else { // Basic or indexed token
        // For indexed tokens we need to create every index in the indexed
        // token key domain if it doesn't exist already or if size or type doesn't match
        for (arrayIndex = 0; arrayIndex < tokenArraySize[i]; arrayIndex++) {
          status = nvm3_getObjectInfo(nvm3_defaultHandle,
                                      tokenNvm3Keys[i] + arrayIndex,
                                      &objectType,
                                      &size);

          if ((status == ECODE_NVM3_ERR_KEY_NOT_FOUND)
              || ((status == ECODE_NVM3_OK)
                  && ((objectType != NVM3_OBJECTTYPE_DATA)
                      || (size != tokenSize[i])))) {
            TOKENDBG(emberSerialPrintf(SER232, "Creating basic/index token (index: %x) from default value\r\n", arrayIndex); )

            // If the object type is wrong we need to delete it before creating the
            // new one with the correct type.
            // Wrong size will be corrected by the data write itself
            if (((status == ECODE_NVM3_OK)
                 && ((objectType != NVM3_OBJECTTYPE_DATA)))) {
              (void) nvm3_deleteObject(nvm3_defaultHandle, tokenNvm3Keys[i] + arrayIndex);
              TOKENDBG(emberSerialPrintf(SER232, "Deleting object of wrong type\r\n"); )
            }

            ecode = nvm3_writeData(nvm3_defaultHandle,
                                   tokenNvm3Keys[i] + arrayIndex,
                                   (const void *) tokenDefaults[i],
                                   tokenSize[i]);
            TOKENDBG(emberSerialPrintf(SER232,
                                       "halStackInitTokens nvm3_writeData length: %d  status: 0x%x\r\n",
                                       tokenSize[i],
                                       ecode); )
            if (ecode != ECODE_NVM3_OK) {
              break;
            }
          }
        }

        if (tokenIsIdx[i]) {
          // We need to delete objects for any indexes that are no longer used.
          for (arrayIndex = tokenArraySize[i]; arrayIndex < 0x7FU; arrayIndex++) {
            status = nvm3_getObjectInfo(nvm3_defaultHandle,
                                        tokenNvm3Keys[i] + arrayIndex,
                                        &objectType,
                                        &size);

            if (status == ECODE_NVM3_ERR_KEY_NOT_FOUND) {
              // If we don't find an object we assume there are no objects at higher indexes
              // and stop looking.
              break;
            } else {
              ecode = nvm3_deleteObject(nvm3_defaultHandle, tokenNvm3Keys[i] + arrayIndex);
              TOKENDBG(emberSerialPrintf(SER232,
                                         "halStackInitTokens Erased unused index %d of token with key 0x%4x\r\n",
                                         arrayIndex,
                                         tokenNvm3Keys[i]); )
              if (ecode != ECODE_NVM3_OK) {
                break;
              }
            }
          }
        }
      }
    }
  }

  tokensActive = ecode == ECODE_NVM3_OK;

// Map NVM3 error codes to EmberStatus codes
  switch (ecode) {
    case ECODE_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS:
      ret = (EmberStatus) EMBER_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS;
      break;
    case ECODE_NVM3_ERR_ALIGNMENT_INVALID:
      ret = (EmberStatus) EMBER_NVM3_ERR_ALIGNMENT_INVALID;
      break;
    case ECODE_NVM3_ERR_SIZE_TOO_SMALL:
      ret = (EmberStatus) EMBER_NVM3_ERR_SIZE_TOO_SMALL;
      break;
    case ECODE_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED:
      ret = (EmberStatus) EMBER_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED;
      break;
    case ECODE_NVM3_OK:
      ret = (EmberStatus) EMBER_SUCCESS;
      break;
    default:
      ret = (EmberStatus) EMBER_NVM3_ERR_TOKEN_INIT;
      break;
  }
  return ret;
}

void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len)
{
  uint8_t offset;
  Ecode_t ecode = ECODE_NVM3_OK;

  if ( token < 256U) {
    if (tokensActive) {
      TOKENDBG(emberSerialPrintf(SER232,
                                 "Get token[%d]: Key=0x%4x Index= 0x%d\r\n",
                                 token,
                                 tokenNvm3Keys[token],
                                 index); )
      if (tokenIsCnt[token]) {
        ecode = nvm3_readCounter(nvm3_defaultHandle,
                                 tokenNvm3Keys[token],
                                 data);
        TOKENDBG(emberSerialPrintf(SER232,
                                   "halInternalGetTokenData nvm3_readCounter value: 0x%4x status: 0x%x\r\n",
                                   *(uint32_t *) data,
                                   ecode); )
      } else { // Basic/indexed token
        offset = index == 0x7FU ? 0U : index;
        ecode = nvm3_readData(nvm3_defaultHandle,
                              tokenNvm3Keys[token] + offset,
                              data,
                              len);
        TOKENDBG(emberSerialPrintf(SER232,
                                   "halInternalGetTokenData nvm3_readData length: %d status: 0x%x\r\n",
                                   len,
                                   ecode); )
      }
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "halInternalGetTokenData supressed\r\n"); )
    }
  } else {
    #ifdef EMBER_TEST
    assert(false);
    #else //EMBER_TEST
    halInternalGetMfgTokenData(data, token, index, len);
    #endif // EMBER_TEST
  }

  if (ecode != ECODE_NVM3_OK) {
    halNvm3Callback(ecode);
  }
}

// NVM3 doesn't support pointers to data in the NVM storage so we instead copy the data
// to the provided location.
void halInternalGetIdxTokenPtrOrData(void *ptr, uint16_t ID, uint8_t index, uint8_t len)
{
  halInternalGetTokenData(ptr, ID, index, len);
}

void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len)
{
  uint8_t offset;
  Ecode_t ecode = ECODE_NVM3_OK;

  if ( token < 256U) {
    if (tokensActive) {
      TOKENDBG(emberSerialPrintf(SER232,
                                 "Set token[%d]: Key=0x%4x Index= 0x%d\r\n",
                                 token,
                                 tokenNvm3Keys[token],
                                 index); )

      if (tokenIsCnt[token]) {
        ecode = nvm3_writeCounter(nvm3_defaultHandle,
                                  tokenNvm3Keys[token],
                                  *(uint32_t *) data);
        TOKENDBG(emberSerialPrintf(SER232,
                                   "halInternalSetTokenData nvm3_writeCounter value: 0x%4x status: 0x%x\r\n",
                                   *(uint32_t *) data,
                                   ecode); )
      } else { //Basic/indexed token
        offset = index == 0x7FU ? 0U : index;
        ecode = nvm3_writeData(nvm3_defaultHandle,
                               tokenNvm3Keys[token] + offset,
                               data,
                               len);
        TOKENDBG(emberSerialPrintf(SER232,
                                   "halInternalSetTokenData nvm3_writeData length: %d status: 0x%x\r\n",
                                   len,
                                   ecode); )
      }
    } else {
      TOKENDBG(emberSerialPrintf(SER232,
                                 "halInternalSetTokenData supressed\r\n"); )
    }
  } else {
    #ifdef EMBER_TEST
    assert(false);
    #else //EMBER_TEST
    halInternalSetMfgTokenData(token, data, len);
    #endif //EMBER_TEST
  }
  if (ecode != ECODE_NVM3_OK) {
    halNvm3Callback(ecode);
  }
}

void halInternalIncrementCounterToken(uint8_t token)
{
  Ecode_t ecode = ECODE_NVM3_OK;
  if (tokensActive) {
    TOKENDBG(emberSerialPrintf(SER232,
                               "Increment token[%d]: Key=0x%4x\r\n",
                               token,
                               tokenNvm3Keys[token]); )
    ecode = nvm3_incrementCounter(nvm3_defaultHandle, tokenNvm3Keys[token], NULL);
    TOKENDBG(emberSerialPrintf(SER232,
                               "halInternalIncrementCounterToken nvm3_incrementCounter status: 0x%x\r\n",
                               ecode); )
  } else {
    TOKENDBG(emberSerialPrintf(SER232,
                               "halInternalIncrementCounterToken supressed\r\n"); )
  }
  if (ecode != ECODE_NVM3_OK) {
    halNvm3Callback(ecode);
  }
}

#else //USE_NVM3

#include "hal/plugin/sim-eeprom/sim-eeprom.h"

bool tokensActive = false;

EmberStatus halStackInitTokens(void)
{
  tokensActive = true;
  if (halInternalSimEeInit() != EMBER_SUCCESS) {
    TOKENDBG(emberSerialPrintf(SER232, "halInternalSimEeInit Attempt 1 fail\r\n"); )
    if (halInternalSimEeInit() != EMBER_SUCCESS) {
      TOKENDBG(emberSerialPrintf(SER232, "halInternalSimEeInit Attempt 2 fail\r\n"); )
      tokensActive = false;
      return EMBER_SIM_EEPROM_INIT_2_FAILED;
    }
    // *INDENT-OFF*
    TOKENDBG (
    // *INDENT-ON*
  } else {
    emberSerialPrintf(SER232, "halInternalSimEeInit Successful\r\n");
    // *INDENT-OFF*
  )
    // *INDENT-ON*
  }

  #if !defined(BOOTLOADER) && !defined(EMBER_TEST)
  {
    tokTypeStackNvdataVersion tokStack;
    halCommonGetToken(&tokStack, TOKEN_STACK_NVDATA_VERSION);

    if (CURRENT_STACK_TOKEN_VERSION != tokStack) {
      #if defined(DEBUG)
      //Debug images with a bad stack token version should attempt to
      //fix the SimEE before continuing on.
      TOKENDBG(emberSerialPrintf(SER232, "Stack Version mismatch, reloading\r\n"); )
      halInternalSimEeRepair(true);
      if (halInternalSimEeInit() != EMBER_SUCCESS) {
        TOKENDBG(emberSerialPrintf(SER232, "halInternalSimEeInit Attempt 3 fail\r\n"); )
        tokensActive = false;
        return EMBER_SIM_EEPROM_INIT_3_FAILED;
      }
      #else //  node release image
      TOKENDBG(emberSerialPrintf(SER232, "EEPROM_x_VERSION_MISMATCH (%d)\r\n",
                                 EMBER_EEPROM_STACK_VERSION_MISMATCH); )
      tokensActive = false;
        #if defined(PLATFORMTEST) || defined(LEVEL_ONE_TEST)
      tokensActive = true;      //Keep tokens active for test code.
        #endif //defined(PLATFORMTEST)
      return EMBER_EEPROM_STACK_VERSION_MISMATCH;
      #endif
    }
  }
  #endif //!defined(BOOTLOADER) && !defined(EMBER_TEST)

  TOKENDBG(emberSerialPrintf(SER232, "TOKENS ACTIVE\r\n"); )
  return EMBER_SUCCESS;
}

void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len)
{
  if (token < 256) {
    //the token ID is within the SimEEPROM's range, route to the SimEEPROM
    if (tokensActive) {
      halInternalSimEeGetData(data, token, index, len);
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "getIdxToken supressed.\r\n"); )
    }
  } else {
    #ifdef EMBER_TEST
    assert(false);
    #else //EMBER_TEST
    halInternalGetMfgTokenData(data, token, index, len);
    #endif //EMBER_TEST
  }
}

void halInternalGetIdxTokenPtrOrData(void *ptr, uint16_t token, uint8_t index, uint8_t len)
{
  if (token < 256) {
    //the token is within the SimEEPROM's range, route to the SimEEPROM
    if (tokensActive) {
      halInternalSimEeGetPtr(ptr, token, index, len);
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "getIdxToken supressed.\r\n"); )
    }
  } else {
    #ifdef EMBER_TEST
    assert(false);
    #else //EMBER_TEST
    uint32_t *ptrOut = (uint32_t *)ptr;
    uint32_t realAddress = 0;

    //0x7F is a non-indexed token.  Remap to 0 for the address calculation
    index = (index == 0x7F) ? 0 : index;

    if ((token & 0xF000) == (USERDATA_TOKENS & 0xF000)) {
      realAddress = ((USERDATA_BASE | (token & 0x0FFF)) + (len * index));
    } else if ((token & 0xF000) == (LOCKBITSDATA_TOKENS & 0xF000)) {
      realAddress = ((LOCKBITS_BASE | (token & 0x0FFF)) + (len * index));
    } else {
      //This function must only ever be called from token code that passes
      //a proper "token" parameter.  A valid 16bit token must pass the
      //above check to find the 32bit realAddress.
      assert(0);
    }

    *ptrOut = realAddress;
    #endif //EMBER_TEST
  }
}

bool simEeSetDataActiveSemaphore = false;
void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len)
{
  if (token < 256) {
    //the token ID is within the SimEEPROM's range, route to the SimEEPROM
    if (tokensActive) {
      //You cannot interrupt SetData with another SetData!
      assert(!simEeSetDataActiveSemaphore);
      simEeSetDataActiveSemaphore = true;
      halInternalSimEeSetData(token, data, index, len);
      simEeSetDataActiveSemaphore = false;
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "setIdxToken supressed.\r\n"); )
    }
  } else {
    //Something is making a set token call on a manufacturing token,
    //on efm32 we permit this if the token is otherwise unprogrammed
    halInternalSetMfgTokenData(token, data, len);
  }
}

void halInternalIncrementCounterToken(uint8_t token)
{
  if (tokensActive) {
    halInternalSimEeIncrementCounter(token);
  } else {
    TOKENDBG(emberSerialPrintf(SER232, "IncrementCounter supressed.\r\n"); )
  }
}

#endif //USE_NVM3

#ifndef EMBER_TEST

// The following interfaces are admittedly code space hogs but serve
// as glue interfaces to link creator codes to tokens for test code.

uint16_t getTokenAddress(uint16_t creator)
{
  #define DEFINETOKENS
  switch (creator) {
    #define TOKEN_MFG TOKEN_DEF
    #define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  case creator: return TOKEN_##name;
    #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
    #include "stack/config/token-stack.h"
    #undef TOKEN_MFG
    #undef TOKEN_DEF
    default: {
    }
  }
  ;
  #undef DEFINETOKENS
  return INVALID_EE_ADDRESS;
}

uint8_t getTokenSize(uint16_t creator)
{
  #define DEFINETOKENS
  switch (creator) {
    #define TOKEN_MFG TOKEN_DEF
    #define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  case creator: return sizeof(type);
    #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
    #include "stack/config/token-stack.h"
    #undef TOKEN_MFG
    #undef TOKEN_DEF
    default: {
    }
  }
  ;
  #undef DEFINETOKENS
  return 0;
}

uint8_t getTokenArraySize(uint16_t creator)
{
  #define DEFINETOKENS
  switch (creator) {
    #define TOKEN_MFG TOKEN_DEF
    #define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  case creator: return arraysize;
    #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
    #include "stack/config/token-stack.h"
    #undef TOKEN_MFG
    #undef TOKEN_DEF
    default: {
    }
  }
  ;
  #undef DEFINETOKENS
  return 0;
}

#endif //EMBER_TEST
