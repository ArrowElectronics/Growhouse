// *****************************************************************************
// * comms-hub-function-sub-ghz-cli.c
// *
// *
// * Copyright 2018 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include "app/framework/include/af.h"

#include "comms-hub-function-sub-ghz.h"
#include "../comms-hub-function/comms-hub-function.h"   // the print macros

// External Functions

void emAfPluginCommsHubFunctionSubGhzCliChannelChangeStart(void)
{
  const EmberAfPluginCommsHubFunctionStatus status = emberAfCommsHubFunctionSubGhzStartChannelChangeSequence();
  if (status == EMBER_AF_CHF_STATUS_SUCCESS) {
    emberAfPluginCommsHubFunctionPrintln("OK");
  } else {
    emberAfPluginCommsHubFunctionPrintln("Error: 0x%x", status);
  }
}

void emAfPluginCommsHubFunctionSubGhzCliChannelChangeComplete(void)
{
  const uint8_t page = emberUnsignedCommandArgument(0);
  const uint8_t channel = emberUnsignedCommandArgument(1);
  const uint32_t pageChannelMask = (page << 27 | BIT32(channel)); // note replace as soon as possible with EMBER_PAGE_CHANNEL_MASK_FROM_CHANNEL_NUMBER(page, channel);
  const EmberAfPluginCommsHubFunctionStatus status = emberAfCommsHubFunctionSubGhzCompleteChannelChangeSequence(pageChannelMask);
  if (status == EMBER_AF_CHF_STATUS_SUCCESS) {
    emberAfPluginCommsHubFunctionPrintln("OK");
  } else {
    emberAfPluginCommsHubFunctionPrintln("Error: 0x%x", status);
  }
}
