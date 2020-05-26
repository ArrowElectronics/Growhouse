// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include "af.h"
#include "command-relay.h"
#include "command-relay-local.h"

static void parseDeviceEndpointsFromArguments(EmberAfPluginCommandRelayDeviceEndpoint *inDeviceEndpoint,
                                              EmberAfPluginCommandRelayDeviceEndpoint *outDeviceEndpoint);

void emAfPluginCommandRelayAddCommand(void)
{
  EmberAfPluginCommandRelayDeviceEndpoint inDeviceEndpoint;
  EmberAfPluginCommandRelayDeviceEndpoint outDeviceEndpoint;
  parseDeviceEndpointsFromArguments(&inDeviceEndpoint, &outDeviceEndpoint);

  emberAfPluginCommandRelayAdd(&inDeviceEndpoint, &outDeviceEndpoint);
}

void emAfPluginCommandRelayRemoveCommand(void)
{
  EmberAfPluginCommandRelayDeviceEndpoint inDeviceEndpoint;
  EmberAfPluginCommandRelayDeviceEndpoint outDeviceEndpoint;
  parseDeviceEndpointsFromArguments(&inDeviceEndpoint, &outDeviceEndpoint);

  emberAfPluginCommandRelayRemove(&inDeviceEndpoint, &outDeviceEndpoint);
}

void emAfPluginCommandRelayClearCommand(void)
{
  emberAfPluginCommandRelayClear();
}

void emAfPluginCommandRelaySaveCommand(void)
{
  emberAfPluginCommandRelaySave();
}

void emAfPluginCommandRelayLoadCommand(void)
{
  emberAfPluginCommandRelayLoad();
}

void emAfPluginCommandRelayPrintCommand(void)
{
  emAfPluginCommandRelayPrint();
}

static void parseDeviceEndpointsFromArguments(EmberAfPluginCommandRelayDeviceEndpoint *inDeviceEndpoint,
                                              EmberAfPluginCommandRelayDeviceEndpoint *outDeviceEndpoint)
{
  emberCopyBigEndianEui64Argument(0, inDeviceEndpoint->eui64);
  inDeviceEndpoint->endpoint = (uint8_t)emberUnsignedCommandArgument(1);
  inDeviceEndpoint->clusterId = (uint16_t)emberUnsignedCommandArgument(2);

  emberCopyBigEndianEui64Argument(3, outDeviceEndpoint->eui64);
  outDeviceEndpoint->endpoint = (uint8_t)emberUnsignedCommandArgument(4);
  outDeviceEndpoint->clusterId = (uint16_t)emberUnsignedCommandArgument(5);
}
