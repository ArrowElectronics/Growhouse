// Copyright 2014 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "ez-mode.h"

#ifdef EMBER_AF_LEGACY_CLI
  #error The EZ-Mode Commissioning plugin is not compatible with the legacy CLI.
#endif

#define MAX_CLUSTER_IDS 3
static EmberAfClusterId clusterIds[MAX_CLUSTER_IDS];

// plugin ezmode-commissioning client <endpoint:1> <direction:1> <cluster ids:n>
void emberAfPluginEzModeCommissioningClientCommand(void)
{
  EmberStatus status;
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  EmberAfEzModeCommissioningDirection direction = (EmberAfEzModeCommissioningDirection)emberUnsignedCommandArgument(1);
  uint8_t clusterIdLength;
  uint8_t count = emberCommandArgumentCount() - 2;
  if (MAX_CLUSTER_IDS < count) {
    count = MAX_CLUSTER_IDS;
    emberAfAppPrintln("Cluster ids truncated to %d", count);
  }
  for (clusterIdLength = 0; clusterIdLength < count; clusterIdLength++) {
    clusterIds[clusterIdLength]
      = (EmberAfClusterId)emberUnsignedCommandArgument(clusterIdLength + 2);
  }
  status = emberAfEzmodeClientCommission(endpoint,
                                         direction,
                                         clusterIds,
                                         clusterIdLength);
  UNUSED_VAR(status);
  emberAfAppPrintln("%p 0x%x", "client", status);
}

// plugin ezmode-commissioning server <endpoint:1> <identify time:1>
void emberAfPluginEzModeCommissioningServerCommand(void)
{
  EmberStatus status;
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  uint16_t identifyTimeS = (uint16_t)emberUnsignedCommandArgument(1);
  if (identifyTimeS == 0) {
    identifyTimeS = EMBER_AF_PLUGIN_EZMODE_COMMISSIONING_IDENTIFY_TIMEOUT;
  }
  status = emberAfEzmodeServerCommissionWithTimeout(endpoint, identifyTimeS);
  UNUSED_VAR(status);
  emberAfAppPrintln("%p 0x%x", "server", status);
}
