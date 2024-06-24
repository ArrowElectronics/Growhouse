// Copyright 2016 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"

#include "find-and-bind-target.h"

// -----------------------------------------------------------------------------
// CLI Command Definitions

void emberAfPluginFindAndBindTargetStartCommand(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);

  emberAfPluginFindAndBindTargetStart(endpoint);
}
