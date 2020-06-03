// *******************************************************************
// * tunneling-server-cli.c
// *
// *
// * Copyright 2011 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"
#include "tunneling-server.h"

void emAfPluginTunnelingServerCliTransfer(void);

#if !defined(EMBER_AF_GENERATE_CLI)

void emAfPluginTunnelingServerCliPrint(void);

EmberCommandEntry emberAfPluginTunnelingServerCommands[] = {
  emberCommandEntryAction("transfer", emAfPluginTunnelingServerCliTransfer, "vb",
                          "Transfer data through the tunnel"),
  emberCommandEntryAction("busy", emberAfPluginTunnelingServerToggleBusyCommand, "",
                          "Toggly the busy status of the tunnel"),
  emberCommandEntryAction("print", emAfPluginTunnelingServerCliPrint, "",
                          "Print the list of tunnels"),
  emberCommandEntryTerminator(),
};

#endif // EMBER_AF_GENERATE_CLI

// plugin tunneling-server transfer <tunnel index:2> <data>
void emAfPluginTunnelingServerCliTransfer(void)
{
  uint16_t tunnelIndex = (uint16_t)emberUnsignedCommandArgument(0);
  uint8_t data[255];
  uint16_t dataLen = emberCopyStringArgument(1, data, sizeof(data), false);
  EmberAfStatus status = emberAfPluginTunnelingServerTransferData(tunnelIndex,
                                                                  data,
                                                                  dataLen);
  emberAfTunnelingClusterPrintln("%p 0x%x", "transfer", status);
}
