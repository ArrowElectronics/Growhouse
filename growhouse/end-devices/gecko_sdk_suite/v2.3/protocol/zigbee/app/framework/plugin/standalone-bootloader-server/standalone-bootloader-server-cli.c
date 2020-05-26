// *****************************************************************************
// * standalone-bootloader-server-cli.c
// *
// * This file defines the standalone bootloader server CLI.
// *
// * Copyright 2012 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "app/framework/include/af-types.h"
#include "app/framework/plugin/standalone-bootloader-common/bootloader-protocol.h"
#include "standalone-bootloader-server.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/framework/plugin/ota-common/ota-cli.h"

//------------------------------------------------------------------------------
// Forward declarations

void sendLaunchRequestToEui(void);
void sendLaunchRequestToTarget(void);

//------------------------------------------------------------------------------
// Globals

#ifndef EMBER_AF_GENERATE_CLI
static EmberCommandEntry bootloadCommands[] = {
  emberCommandEntryAction("target", sendLaunchRequestToTarget, "uu",
                          "Sends a launch bootloader request to the currently cached target"),
  emberCommandEntryAction("eui", sendLaunchRequestToEui, "buu",
                          "Send a launch bootloader request to the specified EUI64"),
  emberCommandEntryTerminator(),
};

EmberCommandEntry emberAfPluginStandaloneBootloaderServerCommands[] = {
  emberCommandEntryAction("status", emAfStandaloneBootloaderServerPrintStatus, "",
                          "Prints the current status of the server"),
  emberCommandEntryAction("query", (CommandAction)emberAfPluginStandaloneBootloaderServerBroadcastQuery, "",
                          "Sends a broadcast standalone bootloader query."),
  emberCommandEntrySubMenu("bootload", bootloadCommands,
                           "Sends a request to launch the standalone bootloader"),
  emberCommandEntryAction("print-target", emAfStandaloneBootloaderServerPrintTargetClientInfoCommand, "",
                          "Print the cached info about the current bootload target"),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

//------------------------------------------------------------------------------
// Functions

void sendLaunchRequestToTarget(void)
{
  uint8_t index = (uint8_t)emberUnsignedCommandArgument(0);
  EmberAfOtaImageId id = emAfOtaFindImageIdByIndex(index);
  uint8_t tag =  (uint8_t)emberUnsignedCommandArgument(1);

  emberAfPluginStandaloneBootloaderServerStartClientBootloadWithCurrentTarget(&id, tag);
}

void sendLaunchRequestToEui(void)
{
  EmberEUI64 longId;
  uint8_t index = (uint8_t)emberUnsignedCommandArgument(1);
  EmberAfOtaImageId id = emAfOtaFindImageIdByIndex(index);
  uint8_t tag =  (uint8_t)emberUnsignedCommandArgument(2);
  emberCopyEui64Argument(0, longId);

  emberAfPluginStandaloneBootloaderServerStartClientBootload(longId, &id, tag);
}
// *****************************************************************************
