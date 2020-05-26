// *******************************************************************
// * device-maangement-client-cli.c
// *
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "device-management-client.h"
#include "app/framework/plugin/device-management-server/device-management-common.h"
#ifndef EMBER_AF_GENERATE_CLI
  #error The Device Management Client plugin is not compatible with the legacy CLI.
#endif

void emAfDeviceManagementClientCliPrint(void);

void emAfDeviceManagementClientCliPrint(void)
{
  emberAfDeviceManagementClientPrint();
}
