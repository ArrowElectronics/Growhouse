// Copyright 2015 by Ember Corporation. All rights reserved.              *80*

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"
#include "metering-interface.h"
#include "metering-server.h"

// plugin simple-metering-server profiles <data:1>
void emberAfPluginMeteringServerCliProfiles(void)
{
#if (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_METER_PROFILES != 0)
  emberAfPluginMeteringServerInterfaceEnableProfiles(
    (uint8_t)emberUnsignedCommandArgument(0));
#else
  emberAfCorePrintln("Not applicable for 0 configured profiles.");
#endif
}

void emberAfPluginMeteringServerCliDisableFastPolling(void)
{
  bool setFastPolling = (bool)emberUnsignedCommandArgument(0);
  emberAfPluginMeteringServerEnableFastPolling(setFastPolling);
}
