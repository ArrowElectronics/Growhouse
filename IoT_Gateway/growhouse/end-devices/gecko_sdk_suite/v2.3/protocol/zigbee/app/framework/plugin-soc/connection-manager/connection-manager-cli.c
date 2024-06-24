// *******************************************************************
// * connection-manager-cli.c
// *
// * Command line interface functions for the connection manager plugin
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "connection-manager.h"

void emAfPluginConnectionManagerSetForceShortPollCommand(void)
{
  bool forceShortPoll = (bool)emberUnsignedCommandArgument(0);
  halCommonSetToken(TOKEN_FORCE_SHORT_POLL, &forceShortPoll);
}
