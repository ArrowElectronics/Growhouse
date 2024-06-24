//
// command-handlers-security.h
//
// Author(s): Andrew Keesler
//
// Created March 20, 2015
//
// Prototypes for ZigBee PRO Security command handler functions.
//

//------------------------------------------------------------------------------
// Ezsp Command Handlers

EmberStatus emberAfPluginEzspSecurityHandleKeyCommandCallback(uint8_t index,
                                                              EmberKeyStruct* keyStruct);

EmberStatus emberAfEzspAesMmoHashCommandCallback(EmberAesMmoHashContext* context,
                                                 bool finalize,
                                                 uint8_t length,
                                                 uint8_t* data,
                                                 EmberAesMmoHashContext* returnContext);

EmberStatus emberAfPluginEzspSecurityHandleKeyCommandCallback(uint8_t index,
                                                              EmberKeyStruct* keyStruct);

EmberStatus emberAfEzspRemoveDeviceCommandCallback(EmberNodeId destShort,
                                                   EmberEUI64 destLong,
                                                   EmberEUI64 targetLong);

EmberStatus emberAfEzspUnicastNwkKeyUpdateCommandCallback(EmberNodeId destShort,
                                                          EmberEUI64 destLong,
                                                          EmberKeyData* key);

EmberStatus emberAfEzspAddTransientLinkKeyCommandCallback(EmberEUI64 partnerEui64,
                                                          EmberKeyData* key);

EmberStatus emberAfEzspGetTransientLinkKeyCommandCallback(const EmberEUI64 eui,
                                                          EmberTransientKeyData *transientKeyData);

EmberStatus emberAfEzspUpdateLinkKeyCommandCallback(void);

void emberAfPluginEzspSecurityGetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspSecuritySetValueCommandCallback(EmberAfPluginEzspValueCommandContext *context);
void emberAfPluginEzspSecuritySetValueCommandCallback(EmberAfPluginEzspValueCommandContext* context);
void emberAfPluginEzspSecurityGetConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
void emberAfPluginEzspSecuritySetConfigurationValueCommandCallback(EmberAfPluginEzspConfigurationValueCommandContext* context);
void emberAfPluginEzspSecurityPolicyCommandCallback(EmberAfPluginEzspPolicyCommandContext* context);

EmberJoinDecision emberAfPluginEzspSecurityTrustCenterJoinCallback(EmberNodeId newNodeId,
                                                                   const EmberEUI64 newNodeEui64,
                                                                   EmberDeviceUpdate status,
                                                                   EmberNodeId parentOfNewNode,
                                                                   EzspDecisionId decisionId,
                                                                   EmberJoinDecision joinDecision);
