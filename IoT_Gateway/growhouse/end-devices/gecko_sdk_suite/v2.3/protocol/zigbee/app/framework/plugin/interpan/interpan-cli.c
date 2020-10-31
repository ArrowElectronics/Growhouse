// *****************************************************************************
// * interpan-cli.c
// *
// *
// * Copyright 2015 by Silicon Labs. All rights reserved.
// *****************************************************************************

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"
#include "interpan.h"

#ifndef EMBER_AF_GENERATE_CLI
  #error The Interpan plugin is not compatible with the legacy CLI.
#endif

// Prototypes

void emAfInterpanEnableCommand(void);
void emAfInterpanDisableCommand(void);
void emAfInterpanFragmentTestCommand(void);
void emAfInterpanSetMessageTimeoutCommand(void);

// Global variables

// This is large. It may go away or be refactored for future releases.
uint8_t testMessage[EMBER_AF_PLUGIN_INTERPAN_FRAGMENTATION_MAX_PAYLOAD_SIZE];

// Functions

// Globally enable inter-PAN messages
void emAfInterpanEnableCommand(void)
{
  emAfPluginInterpanSetEnableState(true);
}

// Globally disable inter-PAN messages
void emAfInterpanDisableCommand(void)
{
  emAfPluginInterpanSetEnableState(false);
}

// Test an inter-PAN fragment transmission with a randomly-filled payload
void emAfInterpanFragmentTestCommand(void)
{
  EmberPanId panId = (EmberPanId)emberUnsignedCommandArgument(0);
  EmberEUI64 eui64;
  emberCopyBigEndianEui64Argument(1, eui64);
  uint16_t clusterId = (uint16_t)emberUnsignedCommandArgument(2);
  uint16_t messageLen = (uint16_t)emberUnsignedCommandArgument(3);

  messageLen = (messageLen > sizeof(testMessage))
               ? sizeof(testMessage) : messageLen;

  uint16_t i;
  for (i = 0; i < messageLen; i++) {
    testMessage[i] = (halCommonGetRandom() & 0xFF);
  }

  emberAfCorePrint("Sending inter-PAN message of len %d to ", messageLen);
  emberAfCoreDebugExec(emberAfPrintBigEndianEui64(eui64));
  emberAfCorePrint(" with random values: ");

  EmberStatus status = emberAfSendInterPan(panId,
                                           eui64,
                                           EMBER_NULL_NODE_ID,
                                           0,             // mcast id - unused
                                           clusterId,
                                           SE_PROFILE_ID, // GBCS only
                                           messageLen,
                                           testMessage);

  emberAfCorePrintln("%s (0x%X)",
                     (EMBER_SUCCESS == status) ? "success" : "failure",
                     status);
  emberAfCoreFlush();
  emberSerialBufferTick();
}

// Set the timeout for inter-PAN fragment transmissions
void emAfInterpanSetMessageTimeoutCommand(void)
{
#if defined(EMBER_AF_PLUGIN_INTERPAN_ALLOW_FRAGMENTATION)
  uint16_t timeout = (uint16_t)emberUnsignedCommandArgument(0);
  if (0 == timeout) {
    emberAfCorePrintln("ERR: inter-PAN message timeout of 0 not allowed");
    emberAfCoreFlush();
    emberSerialBufferTick();
    return;
  }
  interpanPluginSetFragmentMessageTimeout(timeout);
#else
  emberAfCorePrintln("ERR: inter-PAN fragmentation feature missing");
#endif //EMBER_AF_PLUGIN_INTERPAN_ALLOW_FRAGMENTATION
}
