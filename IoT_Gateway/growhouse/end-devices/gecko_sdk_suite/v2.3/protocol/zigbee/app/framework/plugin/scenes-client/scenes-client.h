// *******************************************************************
// * scenes-client.h
// *
// *
// * Copyright 2011 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

bool emberAfPluginScenesClientParseAddSceneResponse(const EmberAfClusterCommand *cmd,
                                                    uint8_t status,
                                                    uint16_t groupId,
                                                    uint8_t sceneId);

bool emberAfPluginScenesClientParseViewSceneResponse(const EmberAfClusterCommand *cmd,
                                                     uint8_t status,
                                                     uint16_t groupId,
                                                     uint8_t sceneId,
                                                     uint16_t transitionTime,
                                                     const uint8_t *sceneName,
                                                     const uint8_t *extensionFieldSets);
