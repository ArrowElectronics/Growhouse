// Copyright 2017 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "em4.h"

#ifndef EMBER_AF_GENERATE_CLI
  #error The em4 plugin is not compatible with the legacy CLI.
#endif

void emberAfPluginEm4StatusCommand(void)
{
  if (!emberAfOKToGoToEM4Callback()) {
    emberAfCorePrintln("Inactive");
    emberAfCorePrintln("In order to activate EM4 mode you must implement emberAfOKToGoToEM4Callback() to return True");
  } else {
    emberAfCorePrintln("Active");
  }
}
