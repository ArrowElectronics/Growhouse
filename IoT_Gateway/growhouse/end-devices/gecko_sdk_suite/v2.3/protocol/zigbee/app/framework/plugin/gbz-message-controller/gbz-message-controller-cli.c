// *******************************************************************
// * gbz-message-controller-cli.c
// *
// *
// * Copyright 2014 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"
#include "gbz-message-controller.h"

// Functions

// plugin gbz-message-controller clear
void emAfPluginGbzMessageControllerCliClear(void)
{
}

// plugin gbz-message-controller append
// This function will "look" into the ZCL buffer and copy over the current
// filled command into the GBZ command payload buffer.
void emAfPluginGbzMessageControllerCliAppend(void)
{
}

// plugin gbz-message-controller encryption <encryptionEnabled:1>
void emAfPluginGbzMessageControllerCliEncryption(void)
{
}
