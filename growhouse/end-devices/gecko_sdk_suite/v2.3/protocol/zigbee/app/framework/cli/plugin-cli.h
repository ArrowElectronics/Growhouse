// *******************************************************************
// * plugin-cli.h
// *
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#ifndef SILABS_PLUGIN_CLI_H
#define SILABS_PLUGIN_CLI_H

#include ATTRIBUTE_STORAGE_CONFIGURATION

#ifdef EMBER_AF_GENERATED_PLUGIN_COMMAND_DECLARATIONS
  #define EMBER_AF_PLUGIN_COMMANDS { "plugin", NULL, (PGM_P)emberAfPluginCommands },
extern EmberCommandEntry emberAfPluginCommands[];
EMBER_AF_GENERATED_PLUGIN_COMMAND_DECLARATIONS
#else
  #define EMBER_AF_PLUGIN_COMMANDS
#endif

#endif // SILABS_PLUGIN_CLI_H
