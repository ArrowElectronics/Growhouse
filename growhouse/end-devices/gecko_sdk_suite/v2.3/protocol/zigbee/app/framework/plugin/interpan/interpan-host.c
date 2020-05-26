// *******************************************************************
// * interpan-host.c
// *
// * Host-specific code related to the reception and processing of interpan
// * messages.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "interpan.h"
#include "app/framework/util/af-main.h"

//------------------------------------------------------------------------------

void ezspMacFilterMatchMessageHandler(uint8_t filterIndexMatch,
                                      EmberMacPassthroughType legacyPassthroughType,
                                      uint8_t lastHopLqi,
                                      int8_t lastHopRssi,
                                      uint8_t messageLength,
                                      uint8_t *messageContents)
{
  emAfPluginInterpanProcessMessage(messageLength,
                                   messageContents);
}

EmberStatus emAfPluginInterpanSendRawMessage(uint8_t length, uint8_t* message)
{
  return ezspSendRawMessage(length, message);
}

void emberAfPluginInterpanInitCallback(void)
{
}

void emberAfPluginInterpanNcpInitCallback(bool memoryAllocation)
{
  interpanPluginInit();

  EmberMacFilterMatchData filters[] = {
    EMBER_AF_PLUGIN_INTERPAN_FILTER_LIST
  };
  EzspStatus status;

  if (memoryAllocation) {
    status = ezspSetConfigurationValue(EZSP_CONFIG_MAC_FILTER_TABLE_SIZE,
                                       (sizeof(filters)
                                        / sizeof(EmberMacFilterMatchData)));
    if (status != EZSP_SUCCESS) {
      emberAfAppPrintln("%p%p failed 0x%x",
                        "Error: ",
                        "Sizing MAC filter table",
                        status);
      return;
    }
  } else {
    interpanPluginSetMacMatchFilterEnable(true);
  }
}

void interpanPluginSetMacMatchFilterEnable(bool enable)
{
  EmberMacFilterMatchData filters[] = {
    EMBER_AF_PLUGIN_INTERPAN_FILTER_LIST
  };

  uint8_t value[2 * sizeof(filters) / sizeof(EmberMacFilterMatchData)] = { 0 };

  if (enable) {
    uint8_t i;
    for (i = 0; i < sizeof(value) / 2; i++) {
      value[i * 2]     =  LOW_BYTE(filters[i]);
      value[i * 2 + 1] = HIGH_BYTE(filters[i]);
    }
  }

  EmberStatus status = ezspSetValue(EZSP_VALUE_MAC_FILTER_LIST,
                                    sizeof(value), value);
  if (status != EZSP_SUCCESS) {
    emberAfAppPrintln("ERR: failed %s inter-PAN MAC filter (0x%x)",
                      enable ? "enabling" : "disabling",
                      status);
  }
}

EmberStatus emAfInterpanApsCryptMessage(bool encrypt,
                                        uint8_t* message,
                                        uint8_t* messageLength,
                                        uint8_t apsHeaderEndIndex,
                                        EmberEUI64 remoteEui64)
{
#if defined(EMBER_AF_PLUGIN_INTERPAN_ALLOW_APS_ENCRYPTED_MESSAGES)
  #error Not supported by EZSP
#endif

  // Feature not yet supported on EZSP.
  return EMBER_LIBRARY_NOT_PRESENT;
}
