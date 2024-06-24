// Copyright 2014 Silicon Laboratories, Inc.

#include "af.h"
#include "lv-shutdown.h"

#ifdef EMBER_AF_LEGACY_CLI
  #error The Low Voltage Shutdown plugin is not compatible with the legacy CLI.
#endif

// ****** CLI Command handlers ******
void emberAfPluginLowVoltageShutdownEnableCommand(void)
{
  bool enable = emberUnsignedCommandArgument(0);
  emberAfPluginLowVoltageShutdownEnable(enable);
}

void emberAfPluginLowVoltageShutdownStatusCommand(void)
{
  emberAfAppPrint("Low voltage shutdown logic is ");
  if (emberAfPluginLowVoltageShutdownEnabled()) {
    emberAfAppPrintln("enabled.");
  } else {
    emberAfAppPrintln("disabled.");
  }
}

void emberAfPluginLowVoltageShutdownPrintVddCommand()
{
  emberAfAppPrintln("VDD voltage is %d mV",
                    emberAfPluginLowVoltageShutdownGetVoltage());
}
