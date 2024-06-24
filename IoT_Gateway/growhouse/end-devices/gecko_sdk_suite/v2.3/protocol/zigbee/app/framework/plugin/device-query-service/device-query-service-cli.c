// ****************************************************************************
// * device-query-service-cli.c
// *
// * Copyright 2014 Silicon Laboratories, Inc.                             *80*
// ****************************************************************************

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/plugin/device-query-service/device-query-service.h"
#include "app/framework/plugin/device-database/device-database.h"

//============================================================================
// Globals

//============================================================================
// Forward declarations

//============================================================================

void emAfPluginDeviceQueryServiceEnableDisableCommand(void)
{
  emberAfPluginDeviceQueryServiceEnableDisable(emberCommandName()[0] == 'e');
}

void emAfPluginDeviceQueryServiceStatusCommand(void)
{
  EmberEUI64 currentEui64;
  bool enabled = emberAfPluginDeviceQueryServiceGetEnabledState();
  emberAfPluginDeviceQueryServiceGetCurrentDiscoveryTargetEui64(currentEui64);
  emberAfCorePrintln("Enabled: %p", enabled ? "yes" : "no");
  emberAfCorePrint("Current Discovery Target: ");
  emberAfPrintBigEndianEui64(currentEui64);
  emberAfCorePrintln("");
  const EmberAfDeviceInfo* device = emberAfPluginDeviceDatabaseFindDeviceByEui64(currentEui64);
  emberAfCorePrintln("Status: %p",
                     (device == NULL
                      ? ""
                      : emberAfPluginDeviceDatabaseGetStatusString(device->status)));
}
