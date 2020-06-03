// *****************************************************************************
// * zdo-cli.h
// *
// * CLI commands for sending ZDO messages.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#ifndef SILABS_ZDO_CLI_H
#define SILABS_ZDO_CLI_H

extern EmberCommandEntry zdoCommands[];

#if defined(EMBER_TEST) && !defined(EMBER_AF_ENABLE_TX_ZDO)
  #define EMBER_AF_ENABLE_TX_ZDO
#endif

#ifdef EMBER_AF_ENABLE_TX_ZDO
  #define ZDO_COMMANDS \
  { "zdo", NULL, (PGM_P)zdoCommands },
#else
  #define ZDO_COMMANDS
#endif

#endif // SILABS_ZDO_CLI_H
