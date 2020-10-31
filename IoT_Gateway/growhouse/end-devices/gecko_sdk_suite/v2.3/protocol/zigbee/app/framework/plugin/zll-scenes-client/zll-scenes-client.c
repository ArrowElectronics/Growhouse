// *******************************************************************
// * zll-scenes-client.c
// *
// *
// * Copyright 2011 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "../../include/af.h"
#include "../scenes-client/scenes-client.h"

bool emberAfScenesClusterEnhancedAddSceneResponseCallback(uint8_t status,
                                                          uint16_t groupId,
                                                          uint8_t sceneId)
{
  return emberAfPluginScenesClientParseAddSceneResponse(emberAfCurrentCommand(),
                                                        status,
                                                        groupId,
                                                        sceneId);
}

bool emberAfScenesClusterEnhancedViewSceneResponseCallback(uint8_t status,
                                                           uint16_t groupId,
                                                           uint8_t sceneId,
                                                           uint16_t transitionTime,
                                                           uint8_t *sceneName,
                                                           uint8_t *extensionFieldSets)
{
  return emberAfPluginScenesClientParseViewSceneResponse(emberAfCurrentCommand(),
                                                         status,
                                                         groupId,
                                                         sceneId,
                                                         transitionTime,
                                                         sceneName,
                                                         extensionFieldSets);
}

bool emberAfScenesClusterCopySceneResponseCallback(uint8_t status,
                                                   uint16_t groupIdFrom,
                                                   uint8_t sceneIdFrom)
{
  emberAfScenesClusterPrintln("RX: CopySceneResponse 0x%x, 0x%2x, 0x%x",
                              status,
                              groupIdFrom,
                              sceneIdFrom);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}
