// *******************************************************************
// * zll-level-control-server.h
// *
// *
// * Copyright 2011 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#define EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER_MINIMUM_LEVEL 0x01
#define EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER_MAXIMUM_LEVEL 0xFE

EmberAfStatus emberAfPluginZllLevelControlServerMoveToLevelWithOnOffZllExtensions(const EmberAfClusterCommand *cmd);
bool emberAfPluginZllLevelControlServerIgnoreMoveToLevelMoveStepStop(uint8_t endpoint, uint8_t commandId);
