// File: security-cli.h
//
// Routines to print info about the security keys on the device.
//
// Author(s): Rob Alexander <ralexander@ember.com>
//
// Copyright 2008 by Ember Corporation.  All rights reserved.               *80*

#ifndef SILABS_SECURITY_CLI_H
#define SILABS_SECURITY_CLI_H

extern EmberCommandEntry changeKeyCommands[];

extern EmberKeyData cliPreconfiguredLinkKey;
extern EmberKeyData cliNetworkKey;

void changeKeyCommand(void);
void printKeyInfo(void);

#ifndef EMBER_AF_HAS_SECURITY_PROFILE_NONE
extern EmberCommandEntry emAfSecurityCommands[];
#endif

#endif // SILABS_SECURITY_CLI_H
