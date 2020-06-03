// *****************************************************************************
// * standalone-bootloader-client-ncp.c
// *
// * This file defines the NCP specific client behavior for the Ember
// * proprietary bootloader protocol.
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#include "app/framework/include/af.h"

void emAfStandaloneBootloaderClientGetInfo(uint16_t* bootloaderVersion,
                                           uint8_t* platformId,
                                           uint8_t* microId,
                                           uint8_t* phyId)
{
  *bootloaderVersion = ezspGetStandaloneBootloaderVersionPlatMicroPhy(platformId,
                                                                      microId,
                                                                      phyId);
}

EmberStatus emAfStandaloneBootloaderClientLaunch(void)
{
  EmberStatus status = ezspLaunchStandaloneBootloader(STANDALONE_BOOTLOADER_NORMAL_MODE);
  if (status != EMBER_SUCCESS) {
    return status;
  }

  // Need to wait here until the NCP resets us.
  while (1) {
  }
  ;
}

void emAfStandaloneBootloaderClientGetMfgInfo(uint16_t* mfgIdReturnValue,
                                              uint8_t* boardNameReturnValue)
{
  uint8_t tokenData[2]; // MFG ID is 2-bytes

  ezspGetMfgToken(EZSP_MFG_MANUF_ID, tokenData);
  *mfgIdReturnValue = HIGH_LOW_TO_INT(tokenData[1], tokenData[0]);

  ezspGetMfgToken(EZSP_MFG_BOARD_NAME, boardNameReturnValue);
}

uint32_t emAfStandaloneBootloaderClientGetRandomNumber(void)
{
  // Although we are supposed to return 32-bits, we only return 16.
  // This is because the client only uses 16-bits of the actual data.
  uint16_t value;
  ezspGetRandomNumber(&value);
  return value;
}

#if !defined(EMBER_TEST)

bool emAfPluginStandaloneBootloaderClientCheckBootloader(void)
{
  // We can't check the bootloader type on the NCP.  We just launch
  // the bootloader and hope for the best.
  return true;
}

void emAfStandaloneBootloaderClientGetKey(uint8_t* returnData)
{
  ezspGetMfgToken(EZSP_MFG_BOOTLOAD_AES_KEY, returnData);
}

#else

bool emAfPluginStandaloneBootloaderClientCheckBootloader(void)
{
  // In order to test the high level app messages in simulation, we lie
  // and say the bootloader is the "correct" one.  We can't actually bootload
  // in simulation.
  return true;
}

#endif
